#include "vertex_array.hpp"

#include "log.hpp"

VertexArray::VertexArray() {
    LOG_TRACE("Creating Vertex Array");
    glCreateVertexArrays(1, &m_ArrayID);
}

VertexArray::~VertexArray() {
    LOG_TRACE("Deleting Vertex Array");
    glDeleteVertexArrays(1, &m_ArrayID);
}

void VertexArray::Bind() const {
    LOG_TRACE("Binding Vertex Array");
    glBindVertexArray(m_ArrayID);
}

void VertexArray::Unbind() const {
    LOG_TRACE("Unbinding Vertex Array");
    glBindVertexArray(0);
}

void VertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer> vb) {
    LOG_TRACE("Adding Vertex Buffer");
    glBindVertexArray(m_ArrayID);

    glEnableVertexAttribArray(m_Buffers.size());
    vb->Bind();

    glVertexAttribPointer(       // lots of these values are hardcoded
        m_Buffers.size(),        // index
        vb->GetComponentCount(), // size
        vb->GetType(),           // type
        GL_FALSE,                // normalized
        0,                       // stride
        (void *)0                // offset
    );

    m_Buffers.push_back(vb);
}

void VertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer> ib) {
    LOG_TRACE("Setting Index Buffer");

    glBindVertexArray(m_ArrayID);
    ib->Bind();

    m_IndexBuffer = ib;
}
const std::vector<std::vector<float>> Calculate_TBN(std::vector<float> vertices,
                                                    std::vector<float> uvs,
                                                    std::vector<int> indices) {
    int length = indices.size();
    std::vector<float> T;
    std::vector<float> B;
    std::vector<float> N;
    std::vector<std::vector<float>> TBN;
    for (int i = 0; i < length; i += 3) {
        glm::vec3 pointsPosition[3];
        glm::vec2 pointsUV[3];
        glm::vec3 edge[2];
        glm::vec2 deltaUV[2];

        for (int j = 0; j < 3; j++) {
            int index = i + j;
            pointsPosition[index].x = vertices[index * 3];
            pointsPosition[index].y = vertices[index * 3 + 1];
            pointsPosition[index].z = vertices[index * 3 + 2];
            pointsUV[index].x = uvs[index * 3];
            pointsUV[index].y = uvs[index * 3 + 1];
        }
        edge[0] = pointsPosition[1] - pointsPosition[0];
        edge[1] = pointsPosition[2] - pointsPosition[0];
        deltaUV[0] = pointsUV[1] - pointsUV[0];
        deltaUV[1] = pointsUV[2] - pointsUV[0];
        float invDet =
            1.0f / (deltaUV[0].x * deltaUV[1].y - deltaUV[0].y * deltaUV[1].x);
        glm::vec3 tangent = glm::normalize(
            invDet * (deltaUV[1].y * edge[0] - deltaUV[0].y * edge[1]));
        glm::vec3 bitangent = glm::normalize(
            invDet * (-deltaUV[1].x * edge[0] + deltaUV[0].x * edge[1]));
        glm::vec3 normal = glm::normalize(glm::cross(edge[0], edge[1]));
        for (int j = 0; j < 3; j++) {
            T.push_back(tangent[j]);
            B.push_back(bitangent[j]);
            N.push_back(normal[j]);
        }
    }
    TBN.push_back(T);
    TBN.push_back(B);
    TBN.push_back(N);
    return TBN;
};