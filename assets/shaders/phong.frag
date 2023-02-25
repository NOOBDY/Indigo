#version 450 core

#define MATH_PI 3.1415926
//so far only one cube map
#define LIGHT_NUMBER 2
#define NONE 0
#define POINT 1
#define SPOT  2
#define DIRECTION 3
#define AMBIENT  4


struct TransformData {
    mat4 transform;

    vec3 position;
    float pad1;

    vec3 rotation;
    float pad2;

    vec3 scale;
    float pad3;

    vec3 direction;
    float pad4;
};

struct LightData {
    TransformData transform;

    vec3 lightColor;
    float radius;

    float power;
    int lightType;
    float innerCone;
    float outerCone;

    mat4 lightProjections[6];

    float nearPlane;
    float farPlane;
    float pad1;
    float pad2;
};

struct MaterialData {
    vec3 baseColor;
    float maxShine;
    // vec3 normal;
};

layout(std140, binding = 1) uniform Materials {
    MaterialData material;
};

layout(std140, binding = 2) uniform Lights {
    LightData lights[LIGHT_NUMBER];
};
layout(location = 0) in vec3 geoPosition;
layout(location = 1)in vec3 worldPosition;
layout(location = 2)in vec3 normal;
layout(location = 3)in vec2 UV;
layout(location = 4)in mat3 TBN;

layout(location = 0) out vec4 color;
// out vec4 color;

uniform vec3 cameraPosition;

uniform sampler2D albedoMap; // samplers are opaque types and
uniform sampler2D normalMap;
uniform sampler2D reflectMap;
// uniform samplerCube shadowMap; // frame buffer texture
uniform samplerCube shadowMap[LIGHT_NUMBER]; // frame buffer texture

vec2 panoramaUV(vec3 dir) {
    dir = normalize(dir);
    return vec2(0.5f + 0.5f * atan(dir.z, dir.x) / MATH_PI, 1.f - acos(dir.y) / MATH_PI);
}
float fade(vec3 center, vec3 position, float radius) {
    return (1 - clamp(length(position - center) / radius, 0, 1));
    // return 1.0 / (length(position - center) / radius + 0.1);
    // return 1.0 - clamp(length(position - center) / radius, 0, 1);
}
vec3 gridSamplingDisk[20] = vec3[](vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1), vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1), vec3(1, 1, 0), vec3(1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0), vec3(1, 0, 1), vec3(-1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1), vec3(0, 1, 1), vec3(0, -1, 1), vec3(0, -1, -1), vec3(0, 1, -1));
float shadow(vec3 position, LightData light, int index) {
    vec3 dir = position - light.transform.position;
    float lightDepth = texture(shadowMap[index], dir).x;
    lightDepth *= light.farPlane;
    float currentDepth = length(dir);
    float shadow = 0.0;
    float bias = 0.15;
    int samples = 20;
    float diskRadius = (1.0 + (currentDepth / light.farPlane)) / 15.0;
    for(int i = 0; i < samples; ++i) {
        float closestDepth = texture(shadowMap[index], dir + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= light.farPlane;   // undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);
    return (shadow);
}
vec3 AllLight(vec3 cameraPosition, vec3 position, LightData light, MaterialData matter, int index) {
    // mesh normal
    vec3 n = normal;
    // n = TBN * (texture(texture3, UV).xyz * 2 - 1);
    vec3 direction = light.transform.direction;
    // direction :light to mesh
    vec3 l = light.lightType == DIRECTION ? direction : normalize(light.transform.position - position);
    // direction :cam to mesh
    vec3 v = normalize(cameraPosition - position);
    // direction :reflection
    vec3 r = normalize(reflect(-l, n));
    // for Blinn-Phong lighting
    vec3 halfwayVec = normalize(v + l);

    float ambient = light.lightType == AMBIENT ? 1.0 : 0.1;
    float fadeOut = light.lightType == POINT ? fade(light.transform.position, position, light.radius) : 1.0;
    // spotlight
    float angle = clamp(dot(direction, l), 0.0, 1.0);
    float spot = light.lightType == SPOT ? clamp((angle - light.outerCone) / (light.innerCone - light.outerCone), 0.0, 1.0) : 1.0;

    // diffuse lighting
    float dotLN = max(dot(halfwayVec, n), 0.0);
    vec3 diffuse = texture(albedoMap, UV).xyz * (dotLN + ambient);

    //color tranform
    // diffuse = pow(diffuse, vec3(2.2));

    // specular lighting
    float dotRV = max(dot(r, v), 0.0);
    // ambient light not specular
    vec3 specular = vec3(1) * ((light.lightType == AMBIENT || diffuse == vec3(0.0)) ? 0.0 : pow(dotRV, material.maxShine));

    float shadow = shadow(position, light, index);
    diffuse *= 1 - shadow;
    specular *= 1 - shadow;

    return (diffuse + specular) * light.lightColor * light.power * fadeOut * spot;
}
vec3 PhongLight(vec3 cameraPosition, vec3 position, LightData lights[LIGHT_NUMBER], MaterialData material) {
    vec3 color3 = vec3(0);
    for(int i = 0; i < LIGHT_NUMBER; i++) {
        LightData light = lights[i];
        if(light.lightType == 0)
            continue;
        color3 += AllLight(cameraPosition, position, light, material, i);
    }

    return color3;
}

vec3 ColorTransform(vec3 color) {
    color = pow(color, vec3(1.0 / 2.2));
    return color;
}

void main() {
    vec3 color3 = vec3(0.);
    // color3 = PhongLight(cameraPosition, worldPosition, lights, material);
    vec3 dir = normalize(worldPosition - cameraPosition);
    vec3 r = reflect(-dir, normal);
    vec2 cuv = panoramaUV(r);
    color3 = texture(reflectMap, cuv).xyz;

    // color3 = texture(texture1, UV).xyz;
    // color3 = ColorTransform(color3);
    color = vec4(color3, 1.);
}