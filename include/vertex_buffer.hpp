#ifndef VERTEX_BUFFER_HPP
#define VERTEX_BUFFER_HPP

#include <GL/glew.h>

class VertexBuffer {
public:
    VertexBuffer(const float vertices[], const int size);
    ~VertexBuffer();

    void Bind();
    void Unbind();

private:
    GLuint m_BufferID;
};

#endif