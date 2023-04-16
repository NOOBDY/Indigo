#include "vertex_buffer.hpp"

#include "log.hpp"

VertexBuffer::VertexBuffer(const std::vector<float> &vertices,
                           const unsigned int typeSize)
    : m_ComponentCount(typeSize / sizeof(GLfloat)), m_Type(GL_FLOAT) {
    glCreateBuffers(1, &m_BufferID);
    glNamedBufferData(m_BufferID, vertices.size() * sizeof(float),
                      vertices.data(), GL_STATIC_DRAW);
    LOG_TRACE("Creating Vertex Buffer {}", m_BufferID);
}

VertexBuffer::VertexBuffer(const float data[], unsigned int size,
                           const unsigned int typeSize)
    : m_ComponentCount(typeSize / sizeof(GLfloat)), m_Type(GL_FLOAT) {
    glCreateBuffers(1, &m_BufferID);
    glNamedBufferData(m_BufferID, size * sizeof(float), data, GL_STATIC_DRAW);
    LOG_TRACE("Creating Vertex Buffer {}", m_BufferID);
}

VertexBuffer::~VertexBuffer() {
    LOG_TRACE("Deleting Vertex Buffer {}", m_BufferID);
    glDeleteBuffers(1, &m_BufferID);
}

void VertexBuffer::Bind() const {
    LOG_TRACE("Binding Vertex Buffer {}", m_BufferID);
    glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
}

void VertexBuffer::Unbind() const {
    LOG_TRACE("Unbinding Vertex Buffer {}", m_BufferID);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
