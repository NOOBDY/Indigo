
#version 450 core

// so far only one cube map
#define LIGHT_NUMBER 2
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
    // TransformData transform;
    mat4 projection;
    mat4 view;
    float nearPlane;
    float farPlane;
    float aspectRatio;
    float FOV;
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
layout(location = 0) in vec3 geoPosition;
layout(location = 1) in vec3 worldPosition;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 UV;
layout(location = 4) in mat3 TBN;

layout(location = 0) out vec4 screenAlbedo;
layout(location = 1) out vec4 screenNormal;
layout(location = 2) out vec4 screenPosition;
// ARM(ao roughtless metallic)
layout(location = 3) out vec4 screenARM;
layout(location = 4) out vec4 screenEmsstion;
// out vec4 color;
uniform vec3 cameraPosition;

uniform sampler2D albedoMap; // samplers are opaque types and
uniform sampler2D normalMap;
uniform sampler2D emisstionMap;
uniform sampler2D reflectMap;
uniform sampler2D ARM;
uniform samplerCube shadowMap[LIGHT_NUMBER]; // frame buffer texture
layout(std140, binding = 0) uniform Matrices {
    mat4 model;
    mat4 viewProjection;
};

void main() {
    vec3 color3 = vec3(0.);
    float maxDepth = 600.0;
    // color3 = PhongLight(cameraPosition, worldPosition, lights, material);
    screenAlbedo.xyz = texture(albedoMap, UV).xyz;
    screenEmsstion.xyz = texture(emisstionMap, UV).xyz;
    // screenEmsstion.xyz=vec3(1,0,0);
    // screenPosition.xyz=(viewProjection*vec4(worldPosition,1.0)).xyz;
    // screenPosition=(viewProjection*model*vec4(geoPosition,1.0))*0.01;
    // screenPosition.w=1.0;
    screenPosition.xyz = (worldPosition / maxDepth + 1.0) * 0.5;
    screenNormal.xyz = normalize(normal);
    // make sure the normalmap is in right format
    if (false) {
        screenNormal.xyz = TBN * (texture(normalMap, UV).xyz * 2 - 1);
    }
    screenARM.xyz = vec3(1.0, 0.5, 0.5);

    // color3 = ColorTransform(color3);
    float len = length(vec3(worldPosition - cameraPosition));
    len /= cameraInfo.farPlane;
    vec4 tem = viewProjection * vec4(worldPosition, 1.0);

    // way1
    //  gl_FragDepth = (tem.z/tem.w)*0.5+0.5;
    // way2
    //  gl_FragDepth = (tem.z/cameraInfo.farPlane+1.0)*0.5;
    //  gl_FragDepth = (len+1.0)*0.5;
}