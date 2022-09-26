#include "light.hpp"

#include "log.hpp"

#pragma pack(16) // std140 layout pads by multiple of 16
Light::Light(glm::vec3 light_Color, float radius, float power,
             LightType light_Type) {
    m_LightColor = light_Color;
    m_Radius = radius;
    m_Power = power;
    m_LightType = light_Type;
}

void Light::SetLightColor(glm::vec3 light_Color) {
    m_LightColor = light_Color;
}

void Light::SetRadius(float radius) {
    m_Radius = radius;
}

void Light::SetPower(float power) {
    m_Power = power;
}

void Light::SetLightType(LightType light_Type) {
    m_LightType = light_Type;
}
void Light::SetCutoff(float cutoff) {
    m_Cutoff = cutoff;
}

LightData Light::GetLightData() {
    LightData data = {
        m_Transform.GetTransformData(),
        m_LightColor,
        m_Radius,
        m_Power,
        m_LightType,
        m_Cutoff,
        1.0,

    };

    return data;
};