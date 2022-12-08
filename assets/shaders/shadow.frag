#version 450 core

#define LIGHT_NUMBER 1

in vec3 normal;
in vec3 geoPosition;
in vec3 worldPosition;
in vec2 UV;
in vec4 test;

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
    float m_NearPlane;
    float m_FarPlane;
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
void main() {
    vec3 color3 = vec3(0.);
    // gl_FragDepth = gl_FragCoord.z;
    float len = length(vec3(worldPosition - lights[0].transform.position));
    len /= lights[0].m_FarPlane;
    // len /= 10;
    color = vec4(vec3(len), 1.0);
    gl_FragDepth = len;

    // color = vec4(worldPosition - lights[0].transform.position, 1.0);
    // color = vec4(normalize(worldPosition - lights[0].transform.position), 1.0);

    // color = vec4(vec3(1), 1.0);
}