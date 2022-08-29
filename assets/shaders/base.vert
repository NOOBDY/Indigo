#version 460 core

layout (location = 0) in vec3 vertPos;

out vec3 fragColor;

layout (std140, binding = 0) uniform Matrices {
    mat4 MVP;
};

void main() {
    gl_Position = MVP * vec4(vertPos, 1);

    fragColor = vertPos;
}