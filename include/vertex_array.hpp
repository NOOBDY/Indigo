#ifndef VERTEX_ARRAY_HPP
#define VERTEX_ARRAY_HPP

#include "pch.hpp"

#include "vertex_buffer.hpp"
#include "index_buffer.hpp"

class VertexArray {
public:
    VertexArray();
    ~VertexArray();

    void Bind() const;
    void Unbind() const;

    void AddVertexBuffer(const std::shared_ptr<VertexBuffer> vb);
    void SetIndexBuffer(const std::shared_ptr<IndexBuffer> ib);

    void LoadOBJ(const std::string &filepath);

    const std::vector<std::shared_ptr<VertexBuffer>> GetVertexBuffers() const {
        return m_Buffers;
    }
    const std::shared_ptr<IndexBuffer> GetIndexBuffer() const {
        return m_IndexBuffer;
    }

private:
    GLuint m_ArrayID;

    std::vector<std::shared_ptr<VertexBuffer>> m_Buffers;
    std::shared_ptr<IndexBuffer> m_IndexBuffer;
};

#endif