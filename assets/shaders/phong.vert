#version 440 core

layout (location = 0) in vec3 vertPos;
layout (location = 1) in vec3 vertNormal;
layout (location = 2) in vec2 vertUV;

out vec3 geo_pos;
out vec3 world_pos;
out vec3 normal;
out vec2 UV;

layout (std140, binding = 0) uniform Matrices {
    mat4 model;
    mat4 viewProjection;
};

void main() {
    gl_Position = viewProjection * model * vec4(vertPos, 1);

    normal = normalize((model * vec4(vertNormal, 1.0)).xyz);
    // normal=normalize(vertNormal);
    geo_pos = vertPos;
    world_pos = (model * vec4(vertPos, 1)).xyz;
    UV = vertUV;
}