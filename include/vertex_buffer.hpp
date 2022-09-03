#ifndef VERTEX_BUFFER_HPP
#define VERTEX_BUFFER_HPP

#include "pch.hpp"

/**
 * Currently, vertex buffers only supports float arrays
 */
class VertexBuffer {
public:
    VertexBuffer(const float vertices[], const unsigned int size,
                 const unsigned int typeSize);
    ~VertexBuffer();

    void Bind() const;
    void Unbind() const;

    unsigned int GetComponentCount() { return m_ComponentCount; }
    GLuint GetType() { return m_Type; }

private:
    GLuint m_BufferID;
    unsigned int m_ComponentCount; // stores the dimensions of the type
                                   //
                                   // e.g. the count of `glm::vec3` would be 3
    GLuint m_Type;
};

#endif