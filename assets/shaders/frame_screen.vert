#version 450 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoords;

// layout(location = 0) out vec2 UV;
out vec2 UV;

layout(std140, binding = 0) uniform Matrices {
    mat4 model;
    mat4 viewProjection;
};
// layout(std140, binding = 1) uniform Materials {
//     MaterialData material;
// };

// layout(std140, binding = 2) uniform Lights {
//     LightData lights[LIGHT_NUMBER];
// };
void main() {
    UV.xy = aTexCoords.xy;
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
}