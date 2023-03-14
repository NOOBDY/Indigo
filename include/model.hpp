#ifndef MODEL_HPP
#define MODEL_HPP

#include "pch.hpp"

#include "texture.hpp"
#include "transform.hpp"
#include "vertex_array.hpp"

class Model {

    int _id;

public:
    enum TextureTypes {
        ALBEDO,
        NORMAL,
        ROUGHNESS,
        SHADOW, // first non-shadow texture index
    };
    struct ModelData {
        TransformData transform;

        int useAlbedoTexture;
        glm::vec3 albedoColor;

        int useEmissionTexture;
        glm::vec3 emissionColor;

        int useARMTexture;
        glm::vec3 ARM;

        int useNormalTexture;
        int id;
        int castShadows;
        int visible;
    };

    Model(std::shared_ptr<VertexArray> vao, Transform transform = Transform());
    ~Model();

    void Draw() const;

    void SetAlbedo(std::shared_ptr<Texture> albedo) { m_Albedo = albedo; }
    void SetNormal(std::shared_ptr<Texture> normal) { m_Normal = normal; }
    void SetRoughness(std::shared_ptr<Texture> roughness) {
        m_Roughness = roughness;
    }
    void SetCastShadows(bool castShadows) { m_CastShadows = castShadows; }
    void SetVisible(bool visible) { m_Visible = visible; }

    std::shared_ptr<Texture> GetAlbedo() { return m_Albedo; }
    std::shared_ptr<Texture> GetNormal() { return m_Normal; }
    std::shared_ptr<Texture> GetRoughness() { return m_Roughness; }
    bool GetCastShadows() { return m_CastShadows; }
    bool GetVisible() { return m_Visible; }

    Transform &GetTransform() { return m_Transform; }
    const Transform &GetTransform() const { return m_Transform; }

    void SetTransform(Transform transform) { m_Transform = transform; }
    const ModelData GetModelData();

private:
    std::shared_ptr<VertexArray> m_VAO;
    Transform m_Transform;

    std::shared_ptr<Texture> m_Albedo;
    std::shared_ptr<Texture> m_Normal;
    std::shared_ptr<Texture> m_Roughness;
    int m_Id;

    bool m_UseAlbedoTexture = true;
    bool m_UseEmissionTexture = true;
    bool m_UseARMTexture = true;
    bool m_UseNormalTexture = true;
    bool m_CastShadows = true;
    bool m_Visible = true;

    glm::vec3 m_AlbedoColor;
    glm::vec3 m_EmissionColor;
    glm::vec3 m_ARM;
};

#endif