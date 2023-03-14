#include "model.hpp"

#include "log.hpp"

#include "renderer.hpp"

Model::Model(std::shared_ptr<VertexArray> vao, Transform transform)
    : m_VAO(vao), m_Transform(transform), m_Id(_id++) {
    LOG_TRACE("Creating Model");
}

Model::~Model() {
    LOG_TRACE("Deleting Model");
}

void Model::Draw() const {
    m_VAO->Bind();

    Renderer::Draw(m_VAO->GetIndexBuffer()->GetCount());
}
const Model::ModelData Model::GetModelData() {
    return ModelData{m_Transform.GetTransformData(),
                     int(m_UseAlbedoTexture),
                     m_AlbedoColor,
                     int(m_UseEmissionTexture),
                     m_EmissionColor,
                     int(m_UseARMTexture),
                     m_ARM,
                     int(m_UseNormalTexture),
                     m_Id,
                     int(m_CastShadows),
                     int(m_Visible)};
}