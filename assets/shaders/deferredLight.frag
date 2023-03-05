#version 450 core

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
struct CameraData {
    // TransformData transform;
    mat4 projection;
    mat4 view;
    float nearPlane;
    float farPlane;
    float aspectRatio;
    float FOV;
};
struct DeferredData {
    vec3 albedo ;
    vec3 normal;
    vec3 position;
    vec3 depth;
};

layout(std140, binding = 0) uniform Matrices {
    mat4 model;
    mat4 viewProjection;
};
layout(std140, binding = 1) uniform Materials {
    MaterialData material;
};

layout(std140, binding = 2) uniform Lights {
    LightData lights[LIGHT_NUMBER];
};
layout(std140, binding = 3) uniform Camera{
    CameraData cameraInfo;
};

layout(location = 0) in vec2 UV;
// in vec2 UV;

layout(location = 0) out vec4 screenLight;
layout(location = 1) out vec4 screenVolume;

uniform vec3 cameraPosition;

uniform sampler2D screenAlbedo;
uniform sampler2D screenNormal;
uniform sampler2D screenPosition;
uniform sampler2D screenARM;
uniform sampler2D screenDepth;
uniform samplerCube shadowMap[LIGHT_NUMBER]; // frame buffer texture
vec3 depth2position(float depth,CameraData info) {
    mat4 viewMatrixInv=inverse(info.view);
    mat4 projectionMatrixInv=inverse(info.projection);
    mat4 invert_view_projection=inverse( info.projection*info.view);
	
    float ViewZ; 
    ViewZ=depth*2.0-1.0;

    //way1
    vec4 sPos = vec4(UV * 2.0 - 1.0,ViewZ, 1.0);
	vec4 worldPosition = invert_view_projection * sPos;
	worldPosition = vec4((worldPosition.xyz / worldPosition.w ), 1.0f);
    // return worldPosition.xyz;


    //way2
    vec4 clipVec = vec4(UV*2.0-1.0, 1,1) * (info.farPlane);
    vec3 viewVec= (projectionMatrixInv*clipVec).xyz;
    vec3 viewPos = viewVec * (ViewZ);
    vec3 worldSpacePosition =(viewMatrixInv*vec4(viewPos,1)).xyz; 
    return worldSpacePosition.xyz;
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
    float bias = 0.9;
    int samples = 20;
    float diskRadius = (1.0 + (currentDepth / light.farPlane)) / 15.0;
    // return shadow;
    // return 1.0-shadow;
    for(int i = 0; i < samples; ++i) {
        float closestDepth = texture(shadowMap[index], dir + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= light.farPlane;   // undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);
    return (shadow);
}
vec4 AllLight(vec3 cameraPosition,DeferredData deferredInfo, LightData light, int index) {
    vec3 position = deferredInfo.position;
    // mesh normal
    vec3 n=deferredInfo.normal;
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
    vec3 diffuse = deferredInfo.albedo * (dotLN + ambient);

    //color tranform
    // diffuse = pow(diffuse, vec3(2.2));

    // specular lighting
    float dotRV = max(dot(r, v), 0.0);
    // ambient light not specular
    vec3 specular = vec3(1) * ((light.lightType == AMBIENT || diffuse == vec3(0.0)) ? 0.0 : pow(dotRV, material.maxShine));

    float shadow = shadow(position, light, index);
    // return vec4(shadow)/LIGHT_NUMBER;

    diffuse *= 1 - shadow;
    specular *= 1 - shadow;

    return vec4((diffuse + specular) * light.lightColor * light.power * fadeOut * spot,shadow);
}
vec4 PhongLight(vec3 cameraPosition, DeferredData deferredInfo,LightData lights[LIGHT_NUMBER]) {
    vec4 color4 = vec4(0);
    for(int i = 0; i < LIGHT_NUMBER; i++) {
        LightData light = lights[i];
        if(light.lightType == 0)
            continue;
        // color3 = light.lightColor;
        color4 += AllLight(cameraPosition,deferredInfo, light, i)/LIGHT_NUMBER;
    }

    return color4;
}

void main() {
    vec3 col=vec3(1.0);

    // vec2 uv=UV;
    DeferredData info;
    info.albedo = texture(screenAlbedo, UV).rgb;
    info.normal= texture(screenNormal, UV).rgb;
    info.depth= texture(screenDepth, UV).rgb;
    info.position=depth2position(info.depth.x,cameraInfo);
    vec3 temPosition= vec4(texture(screenPosition, UV).xyz*2.0-1.0,1.0).xyz;
    temPosition=temPosition* 600.0;


    screenLight.xyz =info.position;
    screenVolume.xyz=temPosition.xyz;
    screenLight.xyz/=600;
    screenVolume.xyz/=600;
    screenVolume-=screenLight;
    screenVolume= abs(screenVolume)* 10.0;
    screenLight=PhongLight(cameraPosition, info, lights);
    screenVolume=screenLight;
    
}