#include "vertex_buffer.hpp"

#include "log.hpp"

VertexBuffer::VertexBuffer(const float vertices[], const int size) {
    glCreateBuffers(1, &m_BufferID);
    LOG_TRACE("Creating Vertex Buffer: 0x{:04x}", m_BufferID);
    glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer() {
    LOG_TRACE("Deleting Vertex Buffer: 0x{:04x}", m_BufferID);
    glDeleteBuffers(1, &m_BufferID);
}

void VertexBuffer::Bind() {
    glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
}

void VertexBuffer::Unbind() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}