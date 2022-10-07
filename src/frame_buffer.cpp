#include "frame_buffer.hpp"

#include "log.hpp"

FrameBuffer ::FrameBuffer(const int binding, int nubmer) {
    LOG_TRACE("Creating Frame Buffer");
    glGenFramebuffers(1, &m_BufferID);
}

FrameBuffer ::~FrameBuffer() {
    LOG_TRACE("Deleting Uniform Buffer");
    glDeleteBuffers(1, &m_BufferID);
    // free(m_TextureIDs);
}
// FrameBuffer::m_number();
void FrameBuffer::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_BufferID);
    glBindTexture(GL_TEXTURE_2D, m_TextureIDs);
}

void FrameBuffer::genTexture() {
    glGenTextures(1, &m_TextureIDs);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}