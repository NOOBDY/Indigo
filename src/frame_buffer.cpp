#include "frame_buffer.hpp"

#include "log.hpp"

FrameBuffer::FrameBuffer() {
    LOG_TRACE("Creating Frame Buffer");
    glCreateFramebuffers(1, &m_BufferID);
}

FrameBuffer::~FrameBuffer() {
    LOG_TRACE("Deleting Frame Buffer");
    glDeleteFramebuffers(1, &m_BufferID);
}

void FrameBuffer::Bind() const {
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        LOG_ERROR("Frame Buffer not complete!");
    LOG_TRACE("Binding Frame Buffer");
    glBindFramebuffer(GL_FRAMEBUFFER, m_BufferID);
}

void FrameBuffer::Unbind() const {
    LOG_TRACE("Unbinding Frame Buffer");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::AttachTexture(GLuint textureID, int textureType) {
    LOG_TRACE("Attaching Texture");
    glNamedFramebufferTexture( //
        m_BufferID,            //
        textureType,
        textureID, //
        0          // mipmap level, idk what this does
    );
}