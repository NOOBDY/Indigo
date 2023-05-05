#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "pch.hpp"

#include "scene_object.hpp"
#include "model.hpp"
#include "vertex_array.hpp"
#include "transform.hpp"
#include "texture.hpp"

struct LightData;

class Light final : public SceneObject {
public:
    enum Type {
        NONE,
        POINT,
        SPOT,
        DIRECTION,
        AMBIENT,
    };

    Light(const std::string &label, Type type, Transform transform,
          float power = 1.0f, float radius = 1.0f,
          glm::vec3 lightColor = glm::vec3(1.0f), bool castShadow = true);

    // Draws the sphere representing the light position
    void Draw() const;

    void SetLightColor(glm::vec3 color) { m_Color = color; }
    void SetRadius(float radius) { m_Radius = radius; }
    void SetPower(float power) { m_Power = power; }
    void SetLightType(Type lightType);
    void SetInner(float inner) { m_InnerCone = inner; }
    void SetOuter(float outer) { m_OuterCone = outer; }
    void SetShadowSize(int size) {
        m_ShadowSize = size;
        if (m_ShadowTexture != nullptr) {
            m_ShadowTexture->SetWidth(m_ShadowSize);
            m_ShadowTexture->SetHeight(m_ShadowSize);
        }
    }
    void SetCastShadow(bool castShadow) {
        if (castShadow && !m_ShadowTexture)
            LOG_ERROR("not set shadow texture");
        else
            m_CastShadow = castShadow;
    }
    void SetColorTexture(std::shared_ptr<Texture> texture) {
        m_ColorTexture = texture;
    }
    void SetUseColorTexture(bool v) {
        if (v && !m_ColorTexture)
            LOG_ERROR("not set light Color texture");
        else
            m_useColorTexture = v;
    }

    glm::mat4 GetLightOrth();
    std::vector<glm::mat4> GetLightProjectionCube() const;

    Type GetType() const { return m_Type; };
    glm::vec3 GetColor() const { return m_Color; };

    float GetRadius() const { return m_Radius; };
    float GetPower() const { return m_Power; };

    float GetInnerCone() const { return m_InnerCone; };
    float GetOuterCone() const { return m_OuterCone; };

    float GetTextureSize() const { return m_ShadowSize; };
    bool GetCastShadow() const { return m_CastShadow; };
    bool GetUseColorTexture() const { return m_useColorTexture; };
    std::shared_ptr<Texture> GetShadowTexture() const {
        return m_ShadowTexture;
    };
    std::shared_ptr<Texture> GetColorTexture() { return m_ColorTexture; }

    LightData GetLightData();
    Texture::Target GetShadowTarget();

    ModelData GetModelData();

private:
    Mesh m_Mesh;
    Type m_Type;

    std::shared_ptr<Texture> m_ShadowTexture;
    std::shared_ptr<Texture> m_ColorTexture;
    glm::vec3 m_Color;

    float m_Radius;
    float m_Power;

    float m_InnerCone;
    float m_OuterCone;

    float m_NearPlane;
    float m_FarPlane;

    bool m_CastShadow;
    int m_ShadowSize;
    bool m_useColorTexture;
};

struct LightData {
    TransformData transform;

    glm::vec3 color;
    float radius;

    float power;
    int type;

    float innerCone;
    float outerCone;

    glm::mat4 projections[6];

    float nearPlane;
    float farPlane;
    // lazy to fix padding issues
    int castShadow;
    int useColorTexture;
};

#endif