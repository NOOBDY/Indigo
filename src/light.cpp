#include "light.hpp"

#pragma pack(16) // std140 layout pads by multiple of 16
Light::Light(Type type, glm::vec3 lightColor, float radius, float power)
    : m_Type(type), m_Color(lightColor), m_Radius(radius), m_Power(power) {}

glm::mat4 Light::GetLightProjection() const {

    glm::mat4 lightProjection =
        glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, m_NearPlane, m_FarPlane);
    // glm::mat4 lightProjection = glm::perspective(
    //     glm::radians(90.0f), (GLfloat)1280 / (GLfloat)720, nearPlane,
    //     farPlane);
    glm::mat4 lightView = glm::lookAt(
        m_Transform.GetPosition(), glm::vec3(0.0f), glm::vec3(0.0, -1.0, 0.0));
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;

    return lightSpaceMatrix;
}

std::vector<glm::mat4> Light::GetLightProjectionCube() const {
    glm::mat4 lightProjection =
        glm::perspective(glm::radians(90.0f), (GLfloat)1024 / (GLfloat)1024,
                         m_NearPlane, m_FarPlane);

    glm::vec3 pos = m_Transform.GetPosition();
    std::vector<glm::mat4> shadowTransforms;
    shadowTransforms.push_back(
        lightProjection * glm::lookAt(pos, pos + glm::vec3(1.0f, 0.0f, 0.0f),
                                      glm::vec3(0.0f, -1.0f, 0.0f)));
    shadowTransforms.push_back(
        lightProjection * glm::lookAt(pos, pos + glm::vec3(-1.0f, 0.0f, 0.0f),
                                      glm::vec3(0.0f, -1.0f, 0.0f)));
    shadowTransforms.push_back(
        lightProjection * glm::lookAt(pos, pos + glm::vec3(0.0f, 1.0f, 0.0f),
                                      glm::vec3(0.0f, 0.0f, 1.0f)));
    shadowTransforms.push_back(
        lightProjection * glm::lookAt(pos, pos + glm::vec3(0.0f, -1.0f, 0.0f),
                                      glm::vec3(0.0f, 0.0f, -1.0f)));
    shadowTransforms.push_back(
        lightProjection * glm::lookAt(pos, pos + glm::vec3(0.0f, 0.0f, 1.0f),
                                      glm::vec3(0.0f, -1.0f, 0.0f)));
    shadowTransforms.push_back(
        lightProjection * glm::lookAt(pos, pos + glm::vec3(0.0f, 0.0f, -1.0f),
                                      glm::vec3(0.0f, -1.0f, 0.0f)));
    return shadowTransforms;
}

LightData Light::GetLightData() {
    std::vector<glm::mat4> lightProjectionVector = GetLightProjectionCube();
    LightData data;

    data.transform = m_Transform.GetTransformData();

    data.color = m_Color;
    data.radius = m_Radius;

    data.power = m_Power;
    data.type = m_Type;

    data.innerCone = m_InnerCone;
    data.outerCone = m_OuterCone;

    for (int i = 0; i < 6; i++)
        data.projections[i] = lightProjectionVector[i];

    data.nearPlane = m_NearPlane;
    data.farPlane = m_FarPlane;

    return data;
}