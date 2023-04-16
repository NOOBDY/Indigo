#include "light.hpp"

#include "importer.hpp"
#include "renderer.hpp"

Light::Light(std::string label, Type type, Transform transform, float power,
             float radius, glm::vec3 lightColor, bool castShadow)
    : SceneObject(SceneObject::LIGHT, label, transform),
      m_Mesh(Importer::LoadFile("../assets/models/sphere.obj")),       //
      m_ShadowTexture(nullptr), m_ColorTexture(nullptr), m_Type(type), //
      m_Color(lightColor), m_Radius(radius),                           //
      m_Power(power), m_InnerCone(20.0f),                              //
      m_OuterCone(30.0f), m_NearPlane(1.0f),                           //
      m_FarPlane(1000.0f),                                             //
      m_CastShadow(castShadow),                                        //
      m_ShadowSize(1024), m_useColorTexture(false) {

    if (m_CastShadow) {
        m_ShadowTexture = std::make_shared<Texture>(
            m_ShadowSize, m_ShadowSize, Texture::DEPTH, GetShadowTarget());
        // LOG_INFO(m_ShadowTexture->GetTextureFormat());
    }
}

void Light::Draw() const {
    m_Mesh[0]->Bind();

    Renderer::Draw(m_Mesh[0]->GetIndexBuffer()->GetCount());
}

void Light::SetLightType(Type lightType) {
    Texture::Target lastTarget = GetShadowTarget();
    m_Type = lightType;
    if (lastTarget != GetShadowTarget()) {
        m_ShadowTexture = std::make_shared<Texture>(
            m_ShadowSize, m_ShadowSize, Texture::DEPTH, GetShadowTarget());
    }
}

glm::mat4 Light::GetLightOrth() {
    float size = m_Radius;

    glm::mat4 lightProjection =
        glm::ortho(-size, size, -size, size, m_NearPlane, m_FarPlane);

    // glm::mat4 lightView = m_Transform.GetDirection4();
    glm::mat4 lightView =
        glm::lookAt(m_Transform.GetPosition(), glm::vec3(0.0, 0.0, 1),
                    glm::vec3(0.0, 1.0, 0.0));
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
    LightData data;

    data.transform = m_Transform.GetTransformData();

    data.color = m_Color;
    data.radius = m_Radius;

    data.power = m_Power;
    data.type = int(m_Type);

    data.innerCone = m_InnerCone;
    data.outerCone = m_OuterCone;
    if (m_Type == DIRECTION)
        data.projections[0] = GetLightOrth();
    else if (m_Type == POINT || m_Type == SPOT) {
        std::vector<glm::mat4> lightProjectionVector;
        lightProjectionVector = GetLightProjectionCube();
        for (int i = 0; i < 6; i++)
            data.projections[i] = lightProjectionVector[i];
    }

    data.nearPlane = m_NearPlane;
    data.farPlane = m_FarPlane;
    // lazy to fix padding issues
    data.castShadow = int(m_CastShadow);
    data.useColorTexture = int(m_useColorTexture);

    return data;
}

Texture::Target Light::GetShadowTarget() {
    switch (m_Type) {
    case Light::NONE:
        throw std::invalid_argument("invalid light type for shadow");
    case Light::POINT:
        return Texture::Target::CUBE;
    case Light::SPOT:
        return Texture::Target::CUBE;
    case Light::DIRECTION:
        return Texture::Target::IMAGE_2D;
    case Light::AMBIENT:
        throw std::invalid_argument("invalid light type for shadow");
    default:
        throw std::invalid_argument("invalid light type for shadow");
    }
}

ModelData Light::GetModelData() {
    /**
     * ! This code is lying
     * ! Turning R from ARM to 0 breaks light sphere shading
     * ! even when the flag is turned off
     */
    return ModelData{
        m_Transform.GetTransformData(), //
        {1, 1, 1},                      // Albedo
        static_cast<int>(false),        //
        {0, 0, 0},                      // Emission
        static_cast<int>(false),        //
        {0, 1, 0},                      // ARM
        static_cast<int>(false),        //
        static_cast<int>(false),        // Normal
        m_ID,                           //
        static_cast<int>(false),        // CastShadow
        static_cast<int>(true),         // Visible
    };
}
