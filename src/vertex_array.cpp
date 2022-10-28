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
const std::vector<std::vector<float>>
Calculate_TBN(std::vector<float> vertices, std::vector<float> uvs,
              std::vector<float> indices) {
    int length = indices.size();
    std::vector<float> T;
    std::vector<float> B;
    std::vector<float> N;
    std::vector<std::vector<float>> TBN;
    for (int i = 0; i < length; i += 3) {
        glm::vec3 points_position[3];
        glm::vec2 points_uv[3];
        for (int j = 0; j < 3; j++) {
            points_position[j].x = 1.0;
        }
    }
    TBN.push_back(T);
    TBN.push_back(B);
    TBN.push_back(N);
    return TBN;
};