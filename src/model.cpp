#include "model.hpp"

#include "log.hpp"

#include "renderer.hpp"

Model::Model(std::shared_ptr<VertexArray> vao, Transform transform)
    : m_VAO(vao), m_Transform(transform) {
    LOG_TRACE("Creating Model");
}

Model::~Model() {
    LOG_TRACE("Deleting Model");
}

void Model::Draw() const {
    m_VAO->Bind();

    Renderer::Draw(m_VAO->GetIndexBuffer()->GetCount());
}