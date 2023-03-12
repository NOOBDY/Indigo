#ifndef MODEL_HPP
#define MODEL_HPP

#include "pch.hpp"

#include "texture.hpp"
#include "transform.hpp"
#include "vertex_array.hpp"

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

    void SetAlbedo(std::shared_ptr<Texture> albedo) { m_Albedo = albedo; }
    void SetNormal(std::shared_ptr<Texture> normal) { m_Normal = normal; }
    void SetRoughness(std::shared_ptr<Texture> roughness) {
        m_Roughness = roughness;
    }

    std::shared_ptr<Texture> GetAlbedo() { return m_Albedo; }
    std::shared_ptr<Texture> GetNormal() { return m_Normal; }
    std::shared_ptr<Texture> GetRoughness() { return m_Roughness; }

    Transform &GetTransform() { return m_Transform; }
    const Transform &GetTransform() const { return m_Transform; }

    void SetTransform(Transform transform) { m_Transform = transform; }

private:
    std::shared_ptr<VertexArray> m_VAO;
    Transform m_Transform;

    std::shared_ptr<Texture> m_Albedo;
    std::shared_ptr<Texture> m_Normal;
    std::shared_ptr<Texture> m_Roughness;
};

#endif