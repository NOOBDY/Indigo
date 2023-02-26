
#version 450 core

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

layout(location = 0) out vec3 screenAlbedo;
layout(location = 1) out vec3 screenNormal;
layout(location = 2) out vec3 screenPosition;
// ARM(ao roughtless metallic)
layout(location = 3) out vec3 screenARM;
// out vec4 color;
uniform vec3 cameraPosition;

uniform sampler2D albedoMap; // samplers are opaque types and
uniform sampler2D normalMap;
uniform sampler2D reflectMap;
uniform sampler2D ARM;
uniform samplerCube shadowMap[LIGHT_NUMBER]; // frame buffer texture
void main() {
    vec3 color3 = vec3(0.);
    float maxDepth=1000;
    // color3 = PhongLight(cameraPosition, worldPosition, lights, material);
    screenAlbedo.xyz=texture(albedoMap,UV).xyz;
    screenPosition.xyz=log(worldPosition)/log(maxDepth);
    // screenPosition.xyz=worldPosition/1000.0;
    // screenPosition.xyz=vec3(1.0);
    screenNormal.xyz= normalize(normal);
    //make sure the normalmap is in right format
    if(texture(normalMap,UV).z>0.5){
        screenNormal.xyz=  TBN * (texture(normalMap, UV).xyz * 2 - 1);
    }
    screenARM.xyz=vec3(1.0,0.5,0.5);
    // color3 = texture(texture1, UV).xyz;
    // color3 = ColorTransform(color3);
    // color = vec4(color3, 1.);
}