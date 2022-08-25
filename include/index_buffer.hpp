#ifndef INDEX_BUFFER_HPP
#define INDEX_BUFFER_HPP

#include <GL/glew.h>

class IndexBuffer {
public:
    IndexBuffer(const uint32_t indices[], const int size);
    ~IndexBuffer();

    void Bind() const;
    void Unbind() const;

    int GetCount() const { return m_Count; }

private:
    GLuint m_BufferID;
    int m_Count;
};

#endif