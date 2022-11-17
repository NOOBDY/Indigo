#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "pch.hpp"

#include "transform.hpp"

enum LightType : int {
    NONE = 0,
    POINT = 1,
    SPOT = 2,
    DIRECTION = 3,
    AMBIENT = 4,
};

struct LightData {
    TransformData transform;

    glm::vec3 lightColor;
    float radius;

    float power;
    LightType lightType;
    float cutoff;
    float temp;
};

class Light {
public:
    Light(glm::vec3 lightColor = glm::vec3(1.0f), float radius = 1.0f,
          float power = 1.0f, LightType lightType = LightType::POINT);

    void SetLightColor(glm::vec3 lightColor);
    void SetRadius(float radius);
    void SetPower(float power);
    void SetLightType(LightType lightType);
    void SetInner(float inner);
    void SetOuter(float outer);
    glm::mat4 GetLightProjection ();

    glm::vec3 GetLightColor() const { return m_LightColor; };
    float GetRadius() const { return m_Radius; };
    float GetPower() const { return m_Power; };
    LightType GetLightType() const { return m_LightType; };
    float GetInner() const { return m_InnerCone; };
    float GetOuter() const { return m_OuterCone; };

    LightData GetLightData();

    Transform m_Transform;

private:
    glm::vec3 m_LightColor;
    float m_Radius = 100000.0f;
    float m_Power = 0.5f;
    LightType m_LightType = LightType::POINT;
    float m_InnerCone = 20.0f;
    float m_OuterCone = 30.0f;
};

#endif