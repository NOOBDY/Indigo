#version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

out vec3 normal;
out vec3 worldPosition;
out vec3 geoPosition;
out vec2 UV;
out mat3 TBN;

in pointData {
    vec3 normal;
    vec3 worldPosition;
    vec3 geoPosition;
    vec2 UV;
    mat4 modelRotation;
    mat4 viewProjection;
} dataIn[];

void setData() {
    // Edges of the triangle
    vec3 edge0 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec3 edge1 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    // Lengths of UV differences
    vec2 deltaUV0 = dataIn[1].UV - dataIn[0].UV;
    vec2 deltaUV1 = dataIn[2].UV - dataIn[0].UV;

    // one over the determinant
    float invDet = 1.0f / (deltaUV0.x * deltaUV1.y - deltaUV1.x * deltaUV0.y);

    vec3 tangent = vec3(invDet * (deltaUV1.y * edge0 - deltaUV0.y * edge1));
    vec3 bitangent = vec3(invDet * (-deltaUV1.x * edge0 + deltaUV0.x * edge1));
    mat4 model = dataIn[0].modelRotation;

    vec3 T = -normalize(vec4(tangent, 0.0).xyz);
    vec3 B = -normalize(vec4(bitangent, 0.0).xyz);
    vec3 N = -normalize(vec4(cross(edge1, edge0), 0.0)).xyz;

    for(int index = 0; index < 3; index++) {
        gl_Position = dataIn[0].viewProjection * gl_in[index].gl_Position;
        normal = dataIn[index].normal;
        worldPosition = dataIn[index].worldPosition;
        geoPosition = dataIn[index].geoPosition;
        UV = dataIn[index].UV;
        TBN = (mat3(T, B, N));

        EmitVertex();
    }
    EndPrimitive();
}
// Default main function
void main() {
    setData();
}