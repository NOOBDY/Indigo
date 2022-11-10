#include "vertex_array.hpp"

#include "log.hpp"

#include "importer.hpp"

VertexArray::VertexArray() {
    LOG_TRACE("Creating Vertex Array");
    glCreateVertexArrays(1, &m_ArrayID);
}

void VertexArray::LoadOBJ(const std::string &filepath) {
    Importer importer(filepath);

    AddVertexBuffer(std::make_shared<VertexBuffer>(importer.GetVertices(),
                                                   3 * sizeof(float)));

    AddVertexBuffer(
        std::make_shared<VertexBuffer>(importer.GetUVs(), 2 * sizeof(float)));

    AddVertexBuffer(std::make_shared<VertexBuffer>(importer.GetNormals(),
                                                   3 * sizeof(float)));

    AddVertexBuffer(std::make_shared<VertexBuffer>(importer.GetTangents(),
                                                   3 * sizeof(float)));

    AddVertexBuffer(std::make_shared<VertexBuffer>(importer.GetBitangents(),
                                                   3 * sizeof(float)));

    SetIndexBuffer(std::make_shared<IndexBuffer>(importer.GetIndices()));
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