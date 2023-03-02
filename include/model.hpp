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

    Model(std::shared_ptr<VertexArray> vao);
    ~Model();

    void Bind();

    void SetAlbedo(std::shared_ptr<Texture> albedo) { m_Albedo = albedo; }
    void SetNormal(std::shared_ptr<Texture> normal) { m_Normal = normal; }
    void SetRoughness(std::shared_ptr<Texture> roughness) {
        m_Roughness = roughness;
    }

    Transform &GetTransform() { return m_Transform; }
    const Transform &GetTransform() const { return m_Transform; }

private:
    std::shared_ptr<VertexArray> m_VAO;
    Transform m_Transform;

    std::shared_ptr<Texture> m_Albedo;
    std::shared_ptr<Texture> m_Normal;
    std::shared_ptr<Texture> m_Roughness;

    std::vector<std::shared_ptr<Texture>> m_Shadows;
};

#endif