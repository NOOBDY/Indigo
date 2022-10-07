#ifndef FRAME_BUFFER_HPP
#define FRAME_BUFFER_HPP

#include "pch.hpp"

class FrameBuffer {
public:
    FrameBuffer(const int binding, int number);
    ~FrameBuffer();
    void bind();
    void genTexture();

private:
    const int m_number = 10;
    GLuint m_BufferID;
    GLuint m_TextureIDs;

    int m_Width = 1024;
    int m_Height = 1024;
};

#endif