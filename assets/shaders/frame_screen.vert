#version 450 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoords;

// layout(location = 0) out vec2 UV;
out vec2 UV;

void main() {
    UV.xy = aTexCoords.xy;
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
}