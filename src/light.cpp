#include "light.hpp"

#include "log.hpp"

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

LightData Light::GetLightData() {
    LightData data = {
        // m_Transform.GetTransformData().transform,
        m_Transform.GetPosition(),
        // m_Transform.GetRotation(),
        // m_Transform.GetScale(),

        m_LightColor,
        m_Radius,
        m_Power,
        m_LightType,
    };

    return data;
};