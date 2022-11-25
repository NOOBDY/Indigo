#version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

out vec3 normal;
out vec3 worldPosition;
out vec3 geoPosition;
out vec2 UV;

in pointData {
    vec3 normal;
    vec3 worldPosition;
    vec3 geoPosition;
    vec2 UV;
    mat4 modelRotation;
    mat4 lightProjections[6];
} dataIn[];

// Default main function
void main() {
    for(int face = 0; face < 6; face++) {
        gl_Layer = face;
        for(int i = 0; i < 3; i++) {

            gl_Position = dataIn[i].lightProjections[face] * vec4(dataIn[i].worldPosition, 1.0);
            // gl_Position = vec4(dataIn[i].worldPosition, 1);
            normal = dataIn[i].normal;
            worldPosition = dataIn[i].worldPosition;
            geoPosition = dataIn[i].geoPosition;
            UV = dataIn[i].UV;
            EmitVertex();
        }
        EndPrimitive();
    }
    // setData();
}