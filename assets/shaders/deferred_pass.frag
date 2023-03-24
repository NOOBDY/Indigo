
#version 450 core

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

struct ModelData {
    TransformData transform;

    vec3 albedoColor;
    int useAlbedoTexture;

    vec3 emissionColor;
    int useEmissionTexture;

    vec3 ARM;
    int useARMTexture;

    int useNormalTexture;
    uint id;
    int castShadows;
    int visible;
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
layout(std140, binding = 0) uniform Matrices {
    mat4 model;
    mat4 viewProjection;
};
layout(std140, binding = 1) uniform ModelInfo {
    ModelData modelInfo;
};

layout(std140, binding = 2) uniform Lights {
    LightData lights;
};

layout(std140, binding = 3) uniform Camera {
    CameraData cameraInfo;
};
layout(location = 0) in vec3 geoPosition;
layout(location = 1) in vec3 worldPosition;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 UV;
layout(location = 4) in mat3 TBN;

layout(location = 0) out vec4 screenAlbedo;
layout(location = 1) out vec4 screenEmission;
layout(location = 2) out vec4 screenNormal;
// ARM(ao roughtless metallic)
layout(location = 3) out vec4 screenARM;
layout(location = 4) out vec4 screenPosition;
layout(location = 5) out vec4 screenID;
// out vec4 color;
// uniform vec3 cameraPosition;

uniform sampler2D albedoMap; // samplers are opaque types and
uniform sampler2D normalMap;
uniform sampler2D emissionMap;
uniform sampler2D reflectMap;
uniform sampler2D ARMMap;
vec3 unpackColor(uint f) {
    vec3 color;
    f /= 256;
    color.r = floor(f / 65536);
    color.g = floor((f - color.r * 65536) / 256.0);
    color.b = floor(f - color.r * 65536 - color.g * 256.0);
    color.xyz /= 256.0;
    return color;
}
uint Hash32(uint x) {
    x ^= x >> 16;
    x *= 0x7feb352dU;
    x ^= x >> 15;
    x *= 0x846ca68bU;
    x ^= x >> 16;
    return x;
}

void main() {
    vec3 color3 = vec3(0.);
    float maxDepth = 600.0;
    screenAlbedo.xyz = (modelInfo.useAlbedoTexture == 1)
                           ? texture(albedoMap, UV).xyz
                           : modelInfo.albedoColor;
    screenEmission.xyz = (modelInfo.useEmissionTexture == 1)
                             ? texture(emissionMap, UV).xyz
                             : modelInfo.emissionColor;
    screenARM.xyz = (modelInfo.useARMTexture == 1) ? texture(ARMMap, UV).xyz
                                                   : modelInfo.ARM;
    screenPosition.xyz = (worldPosition / maxDepth + 1.0) * 0.5;
    screenNormal.xyz = normalize(normal);
    screenID.xyz = unpackColor(Hash32(modelInfo.id));
    screenID.a = modelInfo.id / 256.0;
    // screenID.xyz=vec3(screenID.z);
    // screenID.xyz=vec3(modelInfo.id==3);

    // make sure the normalmap is in right format
    if (modelInfo.useNormalTexture == 1) {
        screenNormal.xyz = TBN * (texture(normalMap, UV).xyz * 2 - 1);
    }

    // color3 = ColorTransform(color3);
    // float len = length(vec3(worldPosition - cameraInfo.transform.position));
    // len /= cameraInfo.farPlane;
    // vec4 tem = viewProjection * vec4(worldPosition, 1.0);

    // way1
    //  gl_FragDepth = (tem.z/tem.w)*0.5+0.5;
    // way2
    //  gl_FragDepth = (tem.z/cameraInfo.farPlane+1.0)*0.5;
    //  gl_FragDepth = (len+1.0)*0.5;
}