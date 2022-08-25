#ifndef VERTEX_BUFFER_HPP
#define VERTEX_BUFFER_HPP

#include <GL/glew.h>

class VertexBuffer {
public:
    VertexBuffer(const float vertices[], const int size);
    ~VertexBuffer();

    void Bind() const;
    void Unbind() const;

private:
    GLuint m_BufferID;
};

#endif