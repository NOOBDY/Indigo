#include "light.hpp"

#include "log.hpp"

#pragma pack(16) // std140 layout pads by multiple of 16
Light::Light(glm::vec3 lightColor, float radius, float power,
             LightType lightType) {
    m_LightColor = lightColor;
    m_Radius = radius;
    m_Power = power;
    m_LightType = lightType;
}

void Light::SetLightColor(glm::vec3 lightColor) {
    m_LightColor = lightColor;
}

void Light::SetRadius(float radius) {
    m_Radius = radius;
}

void Light::SetPower(float power) {
    m_Power = power;
}

void Light::SetLightType(LightType lightType) {
    m_LightType = lightType;
}
void Light::SetInner(float inner) {
    m_InnerCone = inner;
}
void Light::SetOuter(float outer) {
    m_OuterCone = outer;
}

LightData Light::GetLightData() {
    LightData data = {
        m_Transform.GetTransformData(),
        m_LightColor,
        m_Radius,
        m_Power,
        m_LightType,
        m_InnerCone,
        m_OuterCone,

    };

    return data;
};