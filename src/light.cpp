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

glm::mat4 Light::GetLightProjection() {

    float nearPlane = 1.0f;
    float farPlane = 7.5f;
    // //lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); // note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene
    glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearPlane, farPlane);
    glm::mat4 lightView = glm::lookAt(m_Transform.GetPosition(), glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;
    return lightSpaceMatrix;
    // return lightProjection;
};
std::vector<glm::mat4> Light::GetLightProjectionCube() {

    float nearPlane = 1.0f;
    float farPlane = 7.5f;
    glm::mat4 lightProjection = glm::perspective(glm::radians(90.0f), (GLfloat)1280/ (GLfloat)1280, nearPlane, farPlane); // note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene
    // glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearPlane, farPlane);
    glm::vec3 lightPos=m_Transform.GetPosition();
    std::vector<glm::mat4> shadowTransforms;
    shadowTransforms.push_back(lightProjection * glm::lookAt(lightPos, lightPos + glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
    shadowTransforms.push_back(lightProjection * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
    shadowTransforms.push_back(lightProjection * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)));
    shadowTransforms.push_back(lightProjection * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)));
    shadowTransforms.push_back(lightProjection * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
    shadowTransforms.push_back(lightProjection * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
    return shadowTransforms;
};
LightData Light::GetLightData() {
    LightData data = {
        m_Transform.GetTransformData(),
        m_LightColor,
        m_Radius,
        m_Power,
        m_LightType,
        m_InnerCone,
        m_OuterCone,
        this->GetLightProjectionCube(),
    };

    return data;
};