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

layout(std140, binding = 1) uniform Materials {
    MaterialData material;
};

layout(std140, binding = 2) uniform Lights {
    LightData lights[LIGHT_NUMBER];
};
layout(location = 0) in vec2 UV;

layout(location = 0) out vec3 screenLight;
layout(location = 1) out vec3 screenVolume;

uniform sampler2D screenAlbedo;
uniform sampler2D screenNormal;
uniform sampler2D screenPosition;
uniform sampler2D screenARM;
uniform samplerCube shadowMap[LIGHT_NUMBER]; // frame buffer texture

out vec4 FragColor;
void main() {
    vec3 col=vec3(1.0);;

    // col = texture(albedoMap, UV).rgb;

    // col = cube_uv(UV);
    screenLight =col;
}