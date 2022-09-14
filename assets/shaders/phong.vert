#version 440 core

layout(location = 0) in vec3 vertPosition;
layout(location = 1) in vec2 vertUV;
layout(location = 2) in vec3 vertNormal;

out vec3 geoPosition;
out vec3 worldPosition;
out vec3 normal;
out vec2 UV;

layout(std140, binding = 0) uniform Matrices {
    mat4 model;
    mat4 viewProjection;
};

void main() {
    gl_Position = viewProjection * model * vec4(vertPosition, 1);

    mat4 modelRotation = model;

    // reset translation to (0, 0, 0)
    modelRotation[3][0] = 0;
    modelRotation[3][1] = 0;
    modelRotation[3][2] = 0;

    normal = normalize((modelRotation * vec4(vertNormal, 1.0)).xyz);

    geoPosition = vertPosition;
    worldPosition = (model * vec4(vertPosition, 1)).xyz;
    UV = vertUV;
}