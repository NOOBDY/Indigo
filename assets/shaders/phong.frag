#version 460 core

#define LIGHT_NUMBER 2

in vec3 geoPosition;
in vec3 worldPosition;
in vec3 normal;
in vec2 UV;
in mat3 TBN;
// in mat4 modelR;

out vec4 color;

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

uniform sampler2D texture1;// samplers are opaque types and
uniform sampler2D texture2;// cannot exist in uniform blocks
uniform sampler2D texture_n;// cannot exist in uniform blocks

float fade(vec3 center, vec3 postion, float radius) {
    return 1.0 / (length(postion - center) / radius + 0.1);
    // return 1.0 - clamp(length(postion - center) / radius, 0, 1);
};
vec3 AllLight(vec3 cameraPosition, vec3 position, LightData light, MaterialData matter) {
    // mesh normal
    vec3 n = normal;
    // n = (TBN * texture(texture_n, UV).xyz).zyx;
    // n = TBN * texture(texture_n, UV).xyz;
    vec3 direction = light.transform.direction;
    // direction :light to mesh
    vec3 l = light.lightType == 3 ? direction : normalize(light.transform.position - position);
    // direction :cam to mesh
    vec3 v = normalize(cameraPosition - position);
    // direction :reflection
    vec3 r = normalize(reflect(-l, n));
    //for Blinn-Phong lighting
    vec3 halfwayVec = normalize(v + l);

    float ambient = light.lightType == 4 ? 1.0 : 0.1;
    float fadeOut = light.lightType == 2 ? 1.0 : fade(light.transform.position, position, light.radius);
    //spotlight
    float angle = clamp(dot(direction, l), 0.0, 1.0);
    float spot = light.lightType == 2 ? clamp((angle - light.outerCone) / (light.innerCone - light.outerCone), 0.0, 1.0) : 1.0;

    //diffuse lighting
    float dotLN = max(dot(halfwayVec, n), 0.0);
    vec3 diffuse = texture(texture1, UV).xyz * (dotLN + ambient);

    //specular lighting
    float dotRV = max(dot(r, v), 0.0);
    //ambient light not specular
    vec3 specular = vec3(1) * (light.lightType == 4 || diffuse == vec3(0.0) ? 0.0 : pow(dotRV, material.maxShine));

    return (diffuse + specular) * light.lightColor * light.power * fadeOut * spot;
}
vec3 PhongLight(vec3 cameraPosition, vec3 position, LightData lights[LIGHT_NUMBER], MaterialData material) {
    vec3 color3 = vec3(0);
    for(int i = 0; i < LIGHT_NUMBER; i++) {
        LightData light = lights[i];
        if(light.lightType == 0)
            continue;
        color3 += AllLight(cameraPosition, position, light, material);
    }

    return color3;
}

void main() {
    vec3 cameraPosition = vec3(0, 3, 4);
    vec3 color3 = vec3(0.);
    // color3 = texture(texture_n, UV).xyz;
    color3 = (TBN * (texture(texture_n, UV).xyz)).xyz;
    // color3 = normal;
    // color3 = PhongLight(cameraPosition, worldPosition, lights, material);
    // color3 = worldPosition;
    color = vec4(color3, 1.);
}