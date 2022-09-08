#include "vertex_buffer.hpp"

#include "log.hpp"

VertexBuffer::VertexBuffer(const std::vector<float> &vertices,
                           const unsigned int typeSize) {
    LOG_TRACE("Creating Vertex Buffer");
    m_ComponentCount = typeSize / sizeof(float);
    m_Type = GL_FLOAT;
    glCreateBuffers(1, &m_BufferID);
    glNamedBufferData(m_BufferID, vertices.size() * sizeof(float),
                      vertices.data(), GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer() {
    LOG_TRACE("Deleting Vertex Buffer");
    glDeleteBuffers(1, &m_BufferID);
}

void VertexBuffer::Bind() const {
    LOG_TRACE("Binding Vertex Buffer");
    glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
}

void VertexBuffer::Unbind() const {
    LOG_TRACE("Unbinding Vertex Buffer");
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}