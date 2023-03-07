#version 450 core

layout(location = 0) in vec3 vertPosition;
layout(location = 1) in vec2 vertUV;
layout(location = 2) in vec3 vertNormal;
layout(location = 3) in vec3 vertTangent;
layout(location = 4) in vec3 vertBitangent;

layout(location = 0) out vec3 geoPosition;
layout(location = 1) out vec3 worldPosition;
layout(location = 2) out vec3 normal;
layout(location = 3) out vec2 UV;
layout(location = 4) out mat3 TBN;

layout(std140, binding = 0) uniform Matrices {
    mat4 model;
    mat4 viewProjection;
};

void main() {
    // do projection on geo shader
    gl_Position = viewProjection * model * vec4(vertPosition, 1);

    mat4 modelRotation = model;

    // reset translation to (0, 0, 0)
    modelRotation[3][0] = 0;
    modelRotation[3][1] = 0;
    modelRotation[3][2] = 0;

    normal = normalize((modelRotation * vec4(vertNormal, 1.0)).xyz);
    vec3 tangent = normalize(modelRotation * vec4(vertTangent, 0.0)).xyz;
    vec3 bitangent = normalize(modelRotation * vec4(vertBitangent, 0.0)).xyz;
    vec3 normal = normalize(modelRotation * vec4(vertNormal, 0.0)).xyz;

    geoPosition = vertPosition;
    worldPosition = (model * vec4(vertPosition, 1)).xyz;
    UV = vertUV;
    TBN = mat3(tangent, bitangent, normal);
}