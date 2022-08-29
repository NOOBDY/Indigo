#version 460 core

in vec3 fragColor;

out vec3 color;

layout (std140, binding = 1) uniform Data {
    vec3 diffuseColor;
};

void main() {
    color = diffuseColor;
}