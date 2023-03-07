#version 450 core

in vec3 pos;
in vec2 UV;

out vec4 color;

layout(std140, binding = 1) uniform Data {
    vec3 diffuseColor;
};

uniform sampler2D texture1; // samplers are opaque types and
uniform sampler2D texture2; // cannot exist in uniform blocks

void main() {
    if (pos.y < 0)
        color = texture(texture1, UV);
    else
        color = texture(texture2, UV);
}