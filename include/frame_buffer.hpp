#ifndef FRAME_BUFFER_HPP
#define FRAME_BUFFER_HPP

#include "pch.hpp"

class FrameBuffer {
public:
    FrameBuffer(const int binding, int number);
    ~FrameBuffer();
    void bind();

private:
    const int m_number;
    GLuint m_BufferID;
    GLuint m_TextureIDs;

    int m_Width = 1024;
    int m_Height = 1024;
};

#endif