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

    const std::vector<std::shared_ptr<VertexBuffer>> GetVertexBuffers() const {
        return m_Buffers;
    }
    const std::shared_ptr<IndexBuffer> GetIndexBuffer() const {
        return m_IndexBuffer;
    }
    const std::vector<std::vector<float>>
    Calculate_TBN(std::vector<float> vertices, std::vector<float> uvs,
                  std::vector<float> indices);

private:
    GLuint m_ArrayID;

    std::vector<std::shared_ptr<VertexBuffer>> m_Buffers;
    std::shared_ptr<IndexBuffer> m_IndexBuffer;
};

#endif