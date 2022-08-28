#version 460 core

in vec3 fragColor;

out vec3 color;

uniform vec3 diffuseColor;

void main() {
    color = diffuseColor;
}