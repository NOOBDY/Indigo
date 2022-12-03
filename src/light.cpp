#include "light.hpp"

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

glm::mat4 Light::GetLightProjection() {

    glm::mat4 lightProjection =
        glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, m_NearPlane, m_FarPlane);
    // glm::mat4 lightProjection = glm::perspective(
    //     glm::radians(90.0f), (GLfloat)1280 / (GLfloat)720, nearPlane,
    //     farPlane);
    glm::mat4 lightView = glm::lookAt(
        m_Transform.GetPosition(), glm::vec3(0.0f), glm::vec3(0.0, -1.0, 0.0));
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;

    return lightSpaceMatrix;
};
std::vector<glm::mat4> Light::GetLightProjectionCube() {
    glm::mat4 lightProjection =
        glm::perspective(glm::radians(90.0f), (GLfloat)1024 / (GLfloat)1024,
                         m_NearPlane, m_FarPlane);
    // glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f,
    // nearPlane, farPlane);
    glm::vec3 lightPos = m_Transform.GetPosition();
    std::vector<glm::mat4> shadowTransforms;
    shadowTransforms.push_back(
        lightProjection * glm::lookAt(lightPos,
                                      lightPos + glm::vec3(1.0f, 0.0f, 0.0f),
                                      glm::vec3(0.0f, -1.0f, 0.0f)));
    shadowTransforms.push_back(
        lightProjection * glm::lookAt(lightPos,
                                      lightPos + glm::vec3(-1.0f, 0.0f, 0.0f),
                                      glm::vec3(0.0f, -1.0f, 0.0f)));
    shadowTransforms.push_back(
        lightProjection * glm::lookAt(lightPos,
                                      lightPos + glm::vec3(0.0f, 1.0f, 0.0f),
                                      glm::vec3(0.0f, 0.0f, 1.0f)));
    shadowTransforms.push_back(
        lightProjection * glm::lookAt(lightPos,
                                      lightPos + glm::vec3(0.0f, -1.0f, 0.0f),
                                      glm::vec3(0.0f, 0.0f, -1.0f)));
    shadowTransforms.push_back(
        lightProjection * glm::lookAt(lightPos,
                                      lightPos + glm::vec3(0.0f, 0.0f, 1.0f),
                                      glm::vec3(0.0f, -1.0f, 0.0f)));
    shadowTransforms.push_back(
        lightProjection * glm::lookAt(lightPos,
                                      lightPos + glm::vec3(0.0f, 0.0f, -1.0f),
                                      glm::vec3(0.0f, -1.0f, 0.0f)));
    return shadowTransforms;
};
LightData Light::GetLightData() {
    std::vector<glm::mat4> lightProjectionVector =
        this->GetLightProjectionCube();
    LightData data;
    data.transform = m_Transform.GetTransformData();
    data.lightColor = m_LightColor;
    data.radius = m_Radius;
    data.power = m_Power;
    data.lightType = m_LightType;
    data.innerCone = m_InnerCone;
    data.outerCone = m_OuterCone;
    for (int i = 0; i < 6; i++)
        data.lightProjections[i] = lightProjectionVector[i];
    data.m_NearPlane = m_NearPlane;
    data.m_FarPlane = m_FarPlane;

    return data;
};