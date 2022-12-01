#include "vertex_array.hpp"

#include "log.hpp"

VertexArray::VertexArray() {
    glCreateVertexArrays(1, &m_ArrayID);
    LOG_TRACE("Creating Vertex Array {}", m_ArrayID);
}

VertexArray::~VertexArray() {
    LOG_TRACE("Deleting Vertex Array {}", m_ArrayID);
    glDeleteVertexArrays(1, &m_ArrayID);
}

void VertexArray::Bind() const {
    LOG_TRACE("Binding Vertex Array {}", m_ArrayID);
    glBindVertexArray(m_ArrayID);
}

void VertexArray::Unbind() const {
    LOG_TRACE("Unbinding Vertex Array {}", m_ArrayID);
    glBindVertexArray(0);
}

void VertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer> vb) {
    LOG_TRACE("Adding Vertex Buffer");
    glBindVertexArray(m_ArrayID);

    glEnableVertexAttribArray(m_Buffers.size());
    vb->Bind();

    glVertexAttribPointer(       // lots of these values are hardcoded
        m_Buffers.size(),        // index
        vb->GetComponentCount(), // size
        vb->GetType(),           // type
        GL_FALSE,                // normalized
        0,                       // stride
        (void *)0                // offset
    );

    m_Buffers.push_back(vb);
}

void VertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer> ib) {
    LOG_TRACE("Setting Index Buffer");

    glBindVertexArray(m_ArrayID);
    ib->Bind();

    m_IndexBuffer = ib;
}