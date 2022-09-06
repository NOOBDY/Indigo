#version 460 core

layout (location = 0) in vec3 vertPos;
layout (location = 1) in vec2 vertUV;

out vec3 pos;
out vec2 UV;

layout (std140, binding = 0) uniform Matrices {
    mat4 MVP;
};

void main() {
    gl_Position = MVP * vec4(vertPos, 1);

    pos = vertPos;
    UV = vertUV;
}