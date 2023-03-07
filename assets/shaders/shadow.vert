#version 450 core

layout(location = 0) in vec3 vertPosition;
layout(location = 1) in vec2 vertUV;
layout(location = 2) in vec3 vertNormal;

out vec3 normal;
out vec3 worldPosition;
out vec3 geoPosition;
out vec2 UV;
// out mat4 modelR;
#define LIGHT_NUMBER 1

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

out pointData {
    vec3 normal;
    vec3 worldPosition;
    vec3 geoPosition;
    vec2 UV;
    mat4 lightProjections[6];
}
dataOut;
layout(std140, binding = 0) uniform Matrices {
    mat4 model;
    mat4 viewProjection;
};

layout(std140, binding = 2) uniform Lights {
    LightData lights[LIGHT_NUMBER];
};

void main() {
    // do projection on geo shader
    gl_Position = model * vec4(vertPosition, 1);
    // gl_Position = (model * vec4(vertPosition, 1));

    mat4 modelRotation = model;

    // reset translation to (0, 0, 0)
    modelRotation[3][0] = 0;
    modelRotation[3][1] = 0;
    modelRotation[3][2] = 0;

    normal = vertNormal;
    geoPosition = vertPosition;
    worldPosition = (model * vec4(vertPosition, 1)).xyz;
    UV = vertUV;

    dataOut.normal = vertNormal;
    dataOut.worldPosition = worldPosition;
    dataOut.geoPosition = geoPosition;
    dataOut.UV = vertUV;
    // dataOut.lightProjections = lights[0].lightProjections;
    for (int i = 0; i < 6; i++)
        dataOut.lightProjections[i] = lights[0].lightProjections[i];
}