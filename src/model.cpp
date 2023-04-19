#include "model.hpp"

#include "log.hpp"

#include "renderer.hpp"

Model::Model(const std::string &label, const Mesh &mesh, Transform transform)
    : SceneObject(SceneObject::MODEL, label, transform), m_Mesh(mesh) {
    LOG_TRACE("Creating Model");

    m_UseAlbedoTexture = false;
    m_UseEmissionTexture = false;
    m_UseARMTexture = false;
    m_UseNormalTexture = false;
    m_CastShadows = true;
    m_Visible = true;

    m_AlbedoColor = glm::vec3(1, 1, 1);
    m_EmissionColor = glm::vec3(0, 1, 0);
    m_ARM = glm::vec3(1, 0.25, 0.1);
}

Model::~Model() {
    LOG_TRACE("Deleting Model");
}

void Model::Draw() const {
    for (const auto &vao : m_Mesh) {
        vao->Bind();
        Renderer::Draw(vao->GetIndexBuffer()->GetCount());
    }
}

ModelData Model::GetModelData() {
    return ModelData{
        m_Transform.GetTransformData(),
        m_AlbedoColor,
        int(m_UseAlbedoTexture),
        m_EmissionColor,
        int(m_UseEmissionTexture),
        m_ARM,
        int(m_UseARMTexture),

        int(m_UseNormalTexture),
        m_ID,
        int(m_CastShadows),
        int(m_Visible) //
    };
}