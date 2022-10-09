#version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

out vec3 normal;
out vec3 worldPosition;
out vec3 geoPosition;
out vec2 UV;

in pointData {
    vec3 normal;
    vec3 worldPosition;
    vec3 geoPosition;
    vec2 UV;
} dataIn[];

// Default main function
void main() {
    gl_Position = gl_in[0].gl_Position;
    normal = dataIn[0].normal;
    worldPosition = dataIn[0].worldPosition;
    geoPosition = dataIn[0].geoPosition;
    UV = dataIn[0].UV;
    EmitVertex();

    gl_Position = gl_in[1].gl_Position;
    normal = dataIn[1].normal;
    worldPosition = dataIn[1].worldPosition;
    geoPosition = dataIn[1].geoPosition;
    UV = dataIn[1].UV;
    EmitVertex();

    gl_Position = gl_in[2].gl_Position;
    normal = dataIn[2].normal;
    worldPosition = dataIn[2].worldPosition;
    geoPosition = dataIn[2].geoPosition;
    UV = dataIn[2].UV;
    EmitVertex();

    EndPrimitive();
}