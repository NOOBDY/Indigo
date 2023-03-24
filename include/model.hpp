#ifndef MODEL_HPP
#define MODEL_HPP

#include "pch.hpp"

#include "texture.hpp"
#include "transform.hpp"
#include "vertex_array.hpp"

struct ModelData {
    TransformData transform;

    glm::vec3 albedoColor;
    int useAlbedoTexture;

    glm::vec3 emissionColor;
    int useEmissionTexture;

    glm::vec3 ARM;
    int useARMTexture;

    int useNormalTexture;
    unsigned int id;
    int castShadows;
    int visible;
};

class Model {
public:
    enum TextureTypes {
        ALBEDO,
        NORMAL,
        ROUGHNESS,
        SHADOW, // first non-shadow texture index
    };
    Model(std::shared_ptr<VertexArray> vao, Transform transform = Transform());
    ~Model();

    void Draw() const;

    void SetAlbedoTexture(std::shared_ptr<Texture> albedo) {
        m_AlbedoTexture = albedo;
    }
    void SetEmissionTexture(std::shared_ptr<Texture> emission) {
        m_EmissionTexture = emission;
    }
    void SetNormalTexture(std::shared_ptr<Texture> normal) {
        m_NormalTexture = normal;
    }
    void SetARMTexture(std::shared_ptr<Texture> ARM) { m_ARMTexture = ARM; }

    void SetAlbedoColor(glm::vec3 albedo) { m_AlbedoColor = albedo; }
    void SetEmissionColor(glm::vec3 emission) { m_EmissionColor = emission; }

    /// set value between 0-1
    void SetAO(float ao) { m_ARM.x = ao; }
    /// set value between 0-1
    void SetRoughness(float roughness) { m_ARM.y = roughness; }
    /// set value between 0-1
    void SetMetallic(float metallic) { m_ARM.z = metallic; }

    void SetUseAlbedoTexture(bool useAlbedoTexture) {
        m_UseAlbedoTexture = useAlbedoTexture;
    }
    void SetUseEmissionTexture(bool useEmissionTexture) {
        m_UseEmissionTexture = useEmissionTexture;
    }
    void SetUseNormalTexture(bool useNormalTexture) {
        m_UseNormalTexture = useNormalTexture;
    }
    void SetUseARMTexture(bool useARMTexture) {
        m_UseARMTexture = useARMTexture;
    }
    void SetCastShadows(bool castShadows) { m_CastShadows = castShadows; }
    void SetVisible(bool visible) { m_Visible = visible; }

    std::shared_ptr<Texture> GetAlbedoTexture() { return m_AlbedoTexture; }
    std::shared_ptr<Texture> GetEmissionTexture() { return m_EmissionTexture; }
    std::shared_ptr<Texture> GetNormalTexture() { return m_NormalTexture; }
    std::shared_ptr<Texture> GetARMTexture() { return m_ARMTexture; }

    glm::vec3 GetAlbedoColor() { return m_AlbedoColor; }
    glm::vec3 GetEmissionColor() { return m_EmissionColor; }

    float GetAO() { return m_ARM.x; }
    float GetRoughness() { return m_ARM.y; }
    float GetMetallic() { return m_ARM.z; }
    unsigned int GetID() { return m_ID; }

    bool GetUseAlbedoTexture() { return m_UseAlbedoTexture; }
    bool GetUseEmissionTexture() { return m_UseEmissionTexture; }
    bool GetUseNormalTexture() { return m_UseNormalTexture; }
    bool GetUseARMTexture() { return m_UseARMTexture; }
    bool GetCastShadows() { return m_CastShadows; }
    bool GetVisible() { return m_Visible; }

    Transform &GetTransform() { return m_Transform; }
    const Transform &GetTransform() const { return m_Transform; }

    void SetTransform(Transform transform) { m_Transform = transform; }
    /// not const for transform class issues
    ModelData GetModelData();

private:
    std::shared_ptr<VertexArray> m_VAO;
    Transform m_Transform;

    std::shared_ptr<Texture> m_AlbedoTexture;
    std::shared_ptr<Texture> m_EmissionTexture;
    std::shared_ptr<Texture> m_NormalTexture;
    std::shared_ptr<Texture> m_ARMTexture;
    const unsigned int m_ID;

    bool m_UseAlbedoTexture;
    bool m_UseEmissionTexture;
    bool m_UseARMTexture;
    bool m_UseNormalTexture;

    bool m_CastShadows;
    bool m_Visible;

    glm::vec3 m_AlbedoColor;
    glm::vec3 m_EmissionColor;
    glm::vec3 m_ARM;
};

#endif