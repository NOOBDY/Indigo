#include "frame_buffer.hpp"

#include "log.hpp"

FrameBuffer ::FrameBuffer(const int binding, int nubmer) {
    LOG_TRACE("Creating Frame Buffer");
    glGenFramebuffers(1, &m_BufferID);
    m_number(nubmer);

    // m_TextureIDs = malloc(sizeof(GLuint) * number);
}

FrameBuffer ::~FrameBuffer() {
    LOG_TRACE("Deleting Uniform Buffer");
    glDeleteBuffers(1, &m_BufferID);
    // free(m_TextureIDs);
}
void FrameBuffer::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_BufferID);
}
