#version 450 core

#define LIGHT_NUMBER 1

layout(location = 0) in vec3 geoPosition;
layout(location = 1) in vec3 worldPosition;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 UV;
layout(location = 4) in mat3 TBN;

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

uniform sampler2D texture1; // samplers are opaque types and
void main() {}