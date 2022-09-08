#include "index_buffer.hpp"

#include "log.hpp"

IndexBuffer::IndexBuffer(const std::vector<unsigned int> &indices) {
    LOG_TRACE("Creating Index Buffer");
    m_Count = indices.size();
    glCreateBuffers(1, &m_BufferID); // glCreateBuffers is exclusive to GL ^4.5
    glNamedBufferData(m_BufferID, indices.size() * sizeof(unsigned int),
                      indices.data(), GL_STATIC_DRAW);
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