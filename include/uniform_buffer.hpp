#ifndef UNIFORM_BUFFER_HPP
#define UNIFORM_BUFFER_HPP

#include "pch.hpp"

class UniformBuffer {
public:
    UniformBuffer(const int size, const int binding);
    ~UniformBuffer();

    void SetData(const int offset, const int size, const void *data);
    int GetId() { return m_BufferID; };

private:
    GLuint m_BufferID;
};

#endif