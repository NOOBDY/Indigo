#include "index_buffer.hpp"

#include "log.hpp"

IndexBuffer::IndexBuffer(const int indices[], const int size) {
    LOG_TRACE("Creating Index Buffer");
    m_Count = size / sizeof(int);
    glCreateBuffers(1, &m_BufferID); // glCreateBuffers is exclusive to GL ^4.5
    glNamedBufferData(m_BufferID, size, indices, GL_STATIC_DRAW);
}

IndexBuffer::~IndexBuffer() {
    LOG_TRACE("Deleting Index Buffer");
    glDeleteBuffers(1, &m_BufferID);
}

void IndexBuffer::Bind() const {
    LOG_TRACE("Binding Index Buffer");
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
}

void IndexBuffer::Unbind() const {
    LOG_TRACE("Unbinding Index Buffer");
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}