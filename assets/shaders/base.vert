#version 460 core

layout (location = 0) in vec3 vertPos;

out vec3 fragColor;

// uniform mat4 MVP;

void main() {
    gl_Position.xyz = vertPos;
    gl_Position.w = 1.0;

    fragColor = vertPos;
}