#include "uniform_buffer.hpp"

UniformBuffer::UniformBuffer(const int size, const int binding) {
    LOG_TRACE("Creating Uniform Buffer");
    glCreateBuffers(1, &m_BufferID);
    glNamedBufferData(m_BufferID, size, nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_BufferID);
}

UniformBuffer::~UniformBuffer() {
    LOG_TRACE("Deleting Uniform Buffer");
    glDeleteBuffers(1, &m_BufferID);
}

void UniformBuffer::SetData(const int offset, const int size,
                            const void *data) {
    LOG_TRACE("Setting Data");
    glNamedBufferSubData(m_BufferID, offset, size, data);
}