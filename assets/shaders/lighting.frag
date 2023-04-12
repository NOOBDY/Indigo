#version 450 core

#define LIGHT_NUMBER 1
#define NONE 0
#define POINT 1
#define SPOT 2
#define DIRECTION 3
#define AMBIENT 4

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
    TransformData transform;
    mat4 projection;
    mat4 view;
    float nearPlane;
    float farPlane;
    float aspectRatio;
    float FOV;
};
struct DeferredData {
    vec3 albedo;
    vec3 emission;
    vec3 normal;
    vec3 position;
    vec3 ARM;
    vec4 ID;
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
layout(std140, binding = 3) uniform Camera {
    CameraData cameraInfo;
};

layout(location = 0) in vec2 UV;
// in vec2 UV;

layout(location = 0) out vec4 outScreenLight;
layout(location = 1) out vec4 outScreenVolume;

// uniform vec3 cameraPosition;

uniform sampler2D screenAlbedo;
uniform sampler2D screenNormal;
uniform sampler2D screenPosition;
uniform sampler2D screenEmission;
uniform sampler2D screenARM;
uniform sampler2D screenID;
uniform sampler2D screenDepth;

uniform sampler2D screenLight;
uniform sampler2D screenVolume;

uniform sampler2D reflectMap;

uniform sampler2D directionShadowMap; // frame buffer texture
uniform samplerCube pointShadowMap;   // frame buffer texture
// uniform samplerCube shadowMap[LIGHT_NUMBER]; // frame buffer texture
#define PI 3.1415926

vec3 depth2position(highp float depth, mat4 projection, mat4 view) {
    mat4 invert_view_projection = inverse(projection * view);

    float ViewZ;
    ViewZ = depth * 2.0 - 1.0;

    // way1
    vec4 sPos = vec4(UV * 2.0 - 1.0, ViewZ, 1.0);
    vec4 worldPosition = invert_view_projection * sPos;
    worldPosition = vec4((worldPosition.xyz / worldPosition.w), 1.0f);
    return worldPosition.xyz;

    // way2
    //  vec4 clipVec = vec4(UV*2.0-1.0, 1,1) * (info.farPlane);
    //  vec3 viewVec= (projectionMatrixInv*clipVec).xyz;
    //  vec3 viewPos = viewVec * (ViewZ);
    //  vec3 worldSpacePosition =(viewMatrixInv*vec4(viewPos,1)).xyz;
    //  return worldSpacePosition.xyz;
}
float fade(vec3 center, vec3 position, float radius) {
    return (1 - clamp(length(position - center) / radius, 0, 1));
    // return 1.0 / (length(position - center) / radius + 0.1);
    // return 1.0 - clamp(length(position - center) / radius, 0, 1);
}
vec3 gridSamplingDisk[20] =
    vec3[](vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1),
           vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
           vec3(1, 1, 0), vec3(1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
           vec3(1, 0, 1), vec3(-1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1),
           vec3(0, 1, 1), vec3(0, -1, 1), vec3(0, -1, -1), vec3(0, 1, -1));
float pointShadow(vec3 position, LightData light) {
    vec3 dir = position - light.transform.position;
    float lightDepth = texture(pointShadowMap, dir).x;
    lightDepth *= light.farPlane;
    float currentDepth = length(dir);
    float shadow = 0.0;
    float bias = 1.5;
    int samples = 20;
    float diskRadius = (1.0 + (currentDepth / light.farPlane)) / 15.0;

    for (int i = 0; i < samples; ++i) {
        float closestDepth =
            texture(pointShadowMap, dir + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= light.farPlane; // undo mapping [0;1]
        if (currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);
    return (shadow);
}
float directionShadow(vec3 position, vec3 normal, LightData light) {
    vec4 lightSpace = light.lightProjections[0] * vec4(position, 1.0);
    vec3 projCoords = lightSpace.xyz / lightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // keep the shadow at 0.0 when outside the far_plane region of the light's
    // frustum.
    if (projCoords.z > 1.0)
        return  0.0;
    // get closest depth value from light's perspective (using [0,1] range
    // fragPosLight as coords)
    float closestDepth = texture(directionShadowMap, projCoords.xy).r;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    vec3 lightDir = normalize(light.transform.position - position);
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.003);
    float shadow = 0.0;
    shadow += currentDepth - bias > closestDepth ? 1.0 : 0.0;
    return shadow;
    // PCF
    vec2 texelSize = 1.0 / textureSize(directionShadowMap, 0);
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(directionShadowMap,
                                     projCoords.xy + vec2(x, y) * texelSize)
                                 .r;
            shadow += (currentDepth - bias) > (pcfDepth) ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    return shadow;
}
//diffuse
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
vec4 AllLight(vec3 cameraPosition, DeferredData deferredInfo, LightData light,
              int index) {
    vec3 position = deferredInfo.position;
    // mesh normal
    vec3 N = deferredInfo.normal;
    // n = TBN * (texture(texture3, UV).xyz * 2 - 1);
    vec3 direction =normalize( light.transform.direction);
    // direction :light to mesh
    vec3 L = light.lightType == DIRECTION
                 ? direction  
                 : normalize(light.transform.position - position);
    // direction :cam to mesh
    vec3 V = normalize(cameraPosition - position);
    // direction :reflection
    vec3 R = normalize(reflect(-L, N));
    // for Blinn-Phong lighting
    vec3 H = normalize(V + L);

    float ambient = light.lightType == AMBIENT ? 1.0 : 0.05;
    float fadeOut = light.lightType == POINT
                        ? fade(light.transform.position, position, light.radius)
                        : 1.0;
    // spotlight
    float theta     = dot(L, normalize(direction));
    float epsilon   = cos(radians(light.innerCone))- cos(radians(light.outerCone));
    float intensity = clamp((theta - cos(radians(light.outerCone))) / epsilon, 0.0, 1.0);    
    float spot = light.lightType == SPOT
                     ? intensity
                     : 1.0;

    // diffuse lighting
    float dotNL = max(dot(N, L), 0.0);
    float dotNV = max(dot(N, V), 0.0);
    float dotRV = max(dot(R, V), 0.0);
    //shadow
    float shadow = 0;
    if (light.lightType == POINT || light.lightType == SPOT)
        shadow = pointShadow(position, light);
    else if (light.lightType == DIRECTION)
        shadow = directionShadow(position, N, light);

    // color tranform
    //  diffuse = pow(diffuse, vec3(2.2));

    // Cook-Torrance BRDF

    float roughness=deferredInfo.ARM.y;
    float metallic=deferredInfo.ARM.z;
    vec3 albedo=deferredInfo.albedo;
    vec3  F0 = mix (vec3 (0.04), albedo, metallic);
    float NDF = DistributionGGX(N, H, roughness);
    float G   = GeometrySmith(N, V, L, roughness);
    vec3  F   = fresnelSchlick(max(dot(H, V), 0.0), F0 );
    vec3  kD  = vec3(1.0) - F;
    kD *= 1.0 - metallic;	  
    
    vec3  numerator   = NDF * G * F;
    float denominator = 4.0 * max(dotNV, 0.0) *dotNL;
    vec3 specular = numerator / max(denominator, 0.001)*float(deferredInfo.ID.a!=1.0);  
    vec3 diffuse=(kD+ambient) * albedo / PI;
    vec3 Lo=(diffuse + specular)* dotNL*light.lightColor*(1-shadow)*light.power*fadeOut*spot;
    return vec4(Lo,1);
}
vec4 PhongLight(DeferredData deferredInfo, CameraData cameraInfo,
                LightData lights[LIGHT_NUMBER]) {
    vec4 color4 = vec4(0);
    for (int i = 0; i < LIGHT_NUMBER; i++) {
        LightData light = lights[i];
        if (light.lightType == 0)
            continue;
        // color3 = light.lightColor;
        color4 +=
            AllLight(cameraInfo.transform.position, deferredInfo, light, i) /
            LIGHT_NUMBER;
    }

    return color4;
}

void main() {
    vec3 col = vec3(1.0);

    DeferredData baseInfo;
    baseInfo.albedo = texture(screenAlbedo, UV).rgb;
    baseInfo.normal = texture(screenNormal, UV).rgb*2.0-1.0;
    baseInfo.emission = texture(screenEmission, UV).rgb;
    baseInfo.depth = texture(screenDepth, UV).rgb;
    baseInfo.ARM = texture(screenARM, UV).rgb;
    baseInfo.ID = texture(screenID, UV).rgba;
    baseInfo.position = depth2position(baseInfo.depth.x, cameraInfo.projection,
                                       cameraInfo.view);

    outScreenVolume = texture(screenVolume, UV);
    outScreenLight =
        texture(screenLight, UV) + PhongLight(baseInfo, cameraInfo, lights);
}