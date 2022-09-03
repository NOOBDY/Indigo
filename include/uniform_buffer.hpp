#ifndef UNIFORM_BUFFER_HPP
#define UNIFORM_BUFFER_HPP

#include <GL/glew.h>

class UniformBuffer {
public:
    UniformBuffer(const int size, const int binding);
    ~UniformBuffer();

    void SetData(const int offset, const int size, const void *data);

private:
    GLuint m_BufferID;
};

#endif