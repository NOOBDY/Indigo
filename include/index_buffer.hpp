#ifndef INDEX_BUFFER_HPP
#define INDEX_BUFFER_HPP

#include "pch.hpp"

class IndexBuffer {
public:
    IndexBuffer(const std::vector<unsigned int> &indices);
    ~IndexBuffer();

    void Bind() const;
    void Unbind() const;

    int GetCount() const { return m_Count; }

private:
    GLuint m_BufferID;
    int m_Count;
};

#endif