#include "vertex_array.hpp"

#include "log.hpp"

VertexArray::VertexArray() {
    LOG_TRACE("Creating Vertex Array");
    glCreateVertexArrays(1, &m_ArrayID);
}

VertexArray::~VertexArray() {
    LOG_TRACE("Deleting Vertex Array");
    glDeleteVertexArrays(1, &m_ArrayID);
}

void VertexArray::Bind() const {
    LOG_TRACE("Binding Vertex Array");
    glBindVertexArray(m_ArrayID);
}

void VertexArray::Unbind() const {
    LOG_TRACE("Unbinding Vertex Array");
    glBindVertexArray(0);
}

void VertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer> vb) {
    LOG_TRACE("Adding Vertex Buffer");
    glBindVertexArray(m_ArrayID);

    glEnableVertexAttribArray(m_BufferIndex);
    vb->Bind();

    glVertexAttribPointer(       // lots of these values are hardcoded
        m_BufferIndex,           // index
        vb->GetComponentCount(), // size
        vb->GetType(),           // type
        GL_FALSE,                // normalized
        0,                       // stride
        (void *)0                // offset
    );

    m_BufferIndex++;
    m_Buffers.push_back(vb);
}

void VertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer> ib) {
    LOG_TRACE("Setting Index Buffer");

    glBindVertexArray(m_ArrayID);
    ib->Bind();

    m_IndexBuffer = ib;
}