#version 440 core

layout (location = 0) in vec3 vert_pos;
layout (location = 1) in vec2 vert_UV;
layout (location = 2) in vec3 vert_normal;

out vec3 geo_pos;
out vec3 world_pos;
out vec3 normal;
out vec2 UV;

layout (std140, binding = 0) uniform Matrices {
    mat4 model;
    mat4 view_projection;
};

void main() {
    gl_Position = view_projection * model * vec4(vert_pos, 1);

    mat4 model_rotation = model;

    // reset translation to (0, 0, 0)
    model_rotation[3][0] = 0;
    model_rotation[3][1] = 0;
    model_rotation[3][2] = 0;

    normal = normalize((model_rotation * vec4(vert_normal, 1.0)).xyz);

    geo_pos = vert_pos;
    world_pos = (model * vec4(vert_pos, 1)).xyz;
    UV = vert_UV;
}