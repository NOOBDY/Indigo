#version 450 core

layout(location = 0) in vec2 vertPosition;
layout(location = 1) in vec2 vertUV;

layout(location = 0) out vec2 UV;
// out vec2 UV;
void main() {
    UV.xy = vertUV.xy;
    gl_Position = vec4(vertPosition.x, vertPosition.y, 0.0, 1.0);
}