#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "pch.hpp"

#include "transform.hpp"

struct LightData;

class Light {
public:
    enum Type {
        NONE,
        POINT,
        SPOT,
        DIRECTION,
        AMBIENT,
    };

    Light(Type type, glm::vec3 lightColor = glm::vec3(1.0f),
          float radius = 1.0f, float power = 1.0f);

    void SetLightColor(glm::vec3 color) { m_Color = color; }
    void SetRadius(float radius) { m_Radius = radius; }
    void SetPower(float power) { m_Power = power; }
    void SetLightType(Type lightType) { m_Type = lightType; }
    void SetInner(float inner) { m_InnerCone = inner; }
    void SetOuter(float outer) { m_OuterCone = outer; }

    glm::mat4 GetLightProjection() const;
    std::vector<glm::mat4> GetLightProjectionCube() const;

    Type GetType() const { return m_Type; };
    glm::vec3 GetColor() const { return m_Color; };

    float GetRadius() const { return m_Radius; };
    float GetPower() const { return m_Power; };

    float GetInnerCone() const { return m_InnerCone; };
    float GetOuterCone() const { return m_OuterCone; };

    LightData GetLightData();

    Transform &GetTransform() { return m_Transform; }
    const Transform &GetTransform() const { return m_Transform; }

private:
    Type m_Type;

    glm::vec3 m_Color;

    float m_Radius = 200.0f;
    float m_Power = 0.5f;

    float m_InnerCone = 20.0f;
    float m_OuterCone = 30.0f;

    float m_NearPlane = 1;
    float m_FarPlane = 1000.0f;

    Transform m_Transform;
};

struct LightData {
    TransformData transform;

    glm::vec3 color;
    float radius;

    float power;
    Light::Type type;

    float innerCone;
    float outerCone;

    glm::mat4 projections[6];

    float nearPlane;
    float farPlane;
    float pad1;
    float pad2;
};

#endif