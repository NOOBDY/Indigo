#ifndef FRAME_BUFFER_HPP
#define FRAME_BUFFER_HPP

#include "pch.hpp"

#include "texture.hpp"

class FrameBuffer {
public:
    enum Type {
        COLOR_BUFFER,
        DEPTH_BUFFER,
        STENCIL_BUFFER,
    };

    FrameBuffer();
    ~FrameBuffer();

    void Bind() const;
    void Unbind() const;

    void AttachTexture(GLuint textureID, int textureType);

    GLuint GetBufferID() const { return m_BufferID; }

private:
    GLuint m_BufferID;
};

#endif