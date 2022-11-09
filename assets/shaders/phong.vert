#version 440 core

layout(location = 0) in vec3 vertPosition;
layout(location = 1) in vec2 vertUV;
layout(location = 2) in vec3 vertNormal;
layout(location = 3) in vec3 vertTangent;
layout(location = 4) in vec3 vertBitangent;

out vec3 geoPosition;
out vec3 worldPosition;
out vec3 normal;
out vec2 UV;
// out mat4 modelR;

out pointData {
    vec3 normal;
    vec3 worldPosition;
    vec3 geoPosition;
    vec2 UV;
    mat4 modelRotation;
    mat4 viewProjection;
    mat3 TBN;

} dataOut;

layout(std140, binding = 0) uniform Matrices {
    mat4 model;
    mat4 viewProjection;
};

void main() {
    gl_Position = model * vec4(vertPosition, 1);

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
    dataOut.TBN = mat3(tangent, bitangent, normal);
    // modelR = modelRotation;
    dataOut.worldPosition = worldPosition;
    dataOut.geoPosition = geoPosition;
    dataOut.UV = UV;
    dataOut.normal = normal;
    dataOut.modelRotation = modelRotation;
    dataOut.viewProjection = viewProjection;
}