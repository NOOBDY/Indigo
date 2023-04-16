#ifndef MODEL_HPP
#define MODEL_HPP

#include "pch.hpp"

#include "scene_object.hpp"
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
    int id;
    int castShadows;
    int visible;
};

class Model final : public SceneObject {
public:
    enum TextureTypes {
        ALBEDO,
        NORMAL,
        ROUGHNESS,
        SHADOW, // first non-shadow texture index
    };
    Model(std::string label, Mesh mesh, Transform transform = Transform());
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
    void SetARMTexture(std::shared_ptr<Texture> arm) { m_ARMTexture = arm; }

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

    std::shared_ptr<Texture> GetAlbedoTexture() const {
        return m_AlbedoTexture;
    }
    std::shared_ptr<Texture> GetEmissionTexture() const {
        return m_EmissionTexture;
    }
    std::shared_ptr<Texture> GetNormalTexture() const {
        return m_NormalTexture;
    }
    std::shared_ptr<Texture> GetARMTexture() const { return m_ARMTexture; }

    glm::vec3 GetAlbedoColor() const { return m_AlbedoColor; }
    glm::vec3 GetEmissionColor() const { return m_EmissionColor; }

    float GetAO() const { return m_ARM.x; }
    float GetRoughness() const { return m_ARM.y; }
    float GetMetallic() const { return m_ARM.z; }

    bool GetUseAlbedoTexture() const { return m_UseAlbedoTexture; }
    bool GetUseEmissionTexture() const { return m_UseEmissionTexture; }
    bool GetUseNormalTexture() const { return m_UseNormalTexture; }
    bool GetUseARMTexture() const { return m_UseARMTexture; }
    bool GetCastShadows() const { return m_CastShadows; }
    bool GetVisible() const { return m_Visible; }

    /// not const for transform class issues
    ModelData GetModelData();

private:
    Mesh m_Mesh;

    std::shared_ptr<Texture> m_AlbedoTexture;
    std::shared_ptr<Texture> m_EmissionTexture;
    std::shared_ptr<Texture> m_NormalTexture;
    std::shared_ptr<Texture> m_ARMTexture;

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
