#include "pipeline.hpp"

#include <iostream>
#include <random>

#include "renderer.hpp"

Pipeline::Pipeline(int width, int height)
    : m_PointLightShadow("../assets/shaders/shadow_point.vert",
                         "../assets/shaders/shadow_point.geom",
                         "../assets/shaders/shadow_point.frag"),
      m_DirectionLightShadow("../assets/shaders/base_pass.vert",
                             "../assets/shaders/shadow_direction.frag"),
      m_Basic("../assets/shaders/base_pass.vert",
              "../assets/shaders/base_pass.frag"),
      m_SSAO("../assets/shaders/frame_screen.vert",
             "../assets/shaders/ssao.frag"),
      m_Light("../assets/shaders/frame_screen.vert",
              "../assets/shaders/lighting.frag"),
      m_LensFlare("../assets/shaders/frame_screen.vert",
                  "../assets/shaders/lens_flare_and_volume.frag"),
      m_Compositor("../assets/shaders/frame_screen.vert",
                   "../assets/shaders/compositor.frag"),
      m_Width(width), m_Height(height), m_ActivePass(SCREEN), m_UseSSAO(true),
      m_UseOutline(true), m_UseHDRI(true), m_UseToneMap(true),
      m_UseVolume(true), m_VolumeDensity(0.2), m_VolumeColor({1, 1, 1}) {
    m_Passes[NOISE] =
        std::make_shared<Texture>("../assets/textures/perlin_noise.png");

    m_Basic.Bind();
    m_Basic.SetInt("albedoMap", ALBEDO);
    m_Basic.SetInt("emissionMap", EMISSION);
    m_Basic.SetInt("normalMap", NORMAL);
    m_Basic.SetInt("ARMMap", ARM);
    m_Basic.SetInt("reflectMap", REFLECT);

    m_SSAO.Bind();
    m_SSAO.SetInt("screenNormal", NORMAL);
    m_SSAO.SetInt("screenDepth", DEPTH);
    m_SSAO.SetInt("noise", NOISE);

    m_Light.Bind();
    m_Light.SetInt("screenAlbedo", ALBEDO);
    m_Light.SetInt("screenEmission", EMISSION);
    m_Light.SetInt("screenNormal", NORMAL);
    m_Light.SetInt("screenARM", ARM);
    m_Light.SetInt("screenPosition", POSITION);
    m_Light.SetInt("screenID", ID);
    m_Light.SetInt("screenDepth", DEPTH);

    m_Light.SetInt("screenLight", LIGHTING);

    m_Light.SetInt("reflectMap", REFLECT);

    m_Light.SetInt("pointShadowMap", POINT_SHADOW);
    m_Light.SetInt("directionShadowMap", DIRECTION_SHADOW);
    m_Light.SetInt("ssao", SSAO);

    m_LensFlare.Bind();
    m_LensFlare.SetInt("noise", NOISE);
    m_LensFlare.SetInt("screenEmission", EMISSION);
    m_LensFlare.SetInt("screenDepth", DEPTH);
    m_LensFlare.SetInt("pointShadowMap", POINT_SHADOW);
    m_LensFlare.SetInt("directionShadowMap", DIRECTION_SHADOW);

    m_LensFlare.SetInt("screenVolume", VOLUME);
    m_LensFlare.SetInt("screenLensFlare", LENS_FLARE);

    m_Compositor.Bind();
    m_Compositor.SetInt("screenAlbedo", ALBEDO);
    m_Compositor.SetInt("screenEmission", EMISSION);
    m_Compositor.SetInt("screenNormal", NORMAL);
    m_Compositor.SetInt("screenARM", ARM);
    m_Compositor.SetInt("screenPosition", POSITION);
    m_Compositor.SetInt("screenID", ID);
    m_Compositor.SetInt("screenDepth", DEPTH);

    m_Compositor.SetInt("ssao", SSAO);

    m_Compositor.SetInt("reflectMap", REFLECT);
    m_Compositor.SetInt("screenLight", LIGHTING);
    m_Compositor.SetInt("screenVolume", VOLUME);
    m_Compositor.SetInt("screenLensFlare", LENS_FLARE);

    m_UBOs.push_back(std::make_shared<UniformBuffer>(sizeof(MVP), 0));
    m_UBOs.push_back(std::make_shared<UniformBuffer>(sizeof(ModelData), 1));
    m_UBOs.push_back(std::make_shared<UniformBuffer>(sizeof(LightData), 2));
    m_UBOs.push_back(std::make_shared<UniformBuffer>(sizeof(CameraData), 3));
    m_UBOs.push_back(std::make_shared<UniformBuffer>(sizeof(PipelineData), 4));

    // vertices
    m_Screen.AddVertexBuffer(std::make_shared<VertexBuffer>(
        std::vector<float>{
            -1.0f, 1.0f,  //
            -1.0f, -1.0f, //
            1.0f, -1.0f,  //
            1.0f, 1.0f,   //
        },
        2 * sizeof(float)));

    // UV
    m_Screen.AddVertexBuffer(std::make_shared<VertexBuffer>(
        std::vector<float>{
            0.0f, 1.0f, //
            0.0f, 0.0f, //
            1.0f, 0.0f, //
            1.0f, 1.0f, //
        },
        2 * sizeof(float)));

    // Indices
    m_Screen.SetIndexBuffer(
        std::make_shared<IndexBuffer>(std::vector<unsigned int>{
            0, 1, 2, //
            0, 2, 3, //
        }));
}

void Pipeline::Init() {
    unsigned int attachments[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
                                  GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3,
                                  GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5};

#pragma region color buffer
    m_BasicPassFBO.Bind();
    m_Passes[ALBEDO] =
        std::make_shared<Texture>(m_Width, m_Height, Texture::RGBA);
    m_BasicPassFBO.AttachTexture(m_Passes[ALBEDO]->GetTextureID(),
                                 attachments[0]);
    m_Passes[EMISSION] =
        std::make_shared<Texture>(m_Width, m_Height, Texture::RGBA);
    m_BasicPassFBO.AttachTexture(m_Passes[EMISSION]->GetTextureID(),
                                 attachments[1]);
    m_Passes[NORMAL] =
        std::make_shared<Texture>(m_Width, m_Height, Texture::RGBA);
    m_BasicPassFBO.AttachTexture(m_Passes[NORMAL]->GetTextureID(),
                                 attachments[2]);
    m_Passes[ARM] = std::make_shared<Texture>(m_Width, m_Height, Texture::RGBA);
    m_BasicPassFBO.AttachTexture(m_Passes[ARM]->GetTextureID(), attachments[3]);

    m_Passes[POSITION] =
        std::make_shared<Texture>(m_Width, m_Height, Texture::RGBA);
    m_BasicPassFBO.AttachTexture(m_Passes[POSITION]->GetTextureID(),
                                 attachments[4]);
    m_Passes[ID] = std::make_shared<Texture>(m_Width, m_Height, Texture::RGBA,
                                             Texture::IMAGE_2D, 8);
    m_BasicPassFBO.AttachTexture(m_Passes[ID]->GetTextureID(), attachments[5]);
    m_Passes[DEPTH] =
        std::make_shared<Texture>(m_Width, m_Height, Texture::DEPTH);
    m_BasicPassFBO.AttachTexture(m_Passes[DEPTH]->GetTextureID(),
                                 GL_DEPTH_ATTACHMENT);
    glDrawBuffers(6, attachments);
    m_BasicPassFBO.Unbind();
#pragma endregion

#pragma region ssao
    m_SSAOPassFBO.Bind();
    m_Passes[SSAO] =
        std::make_shared<Texture>(m_Width / 2, m_Height / 2, Texture::R);
    m_SSAOPassFBO.AttachTexture(m_Passes[SSAO]->GetTextureID(), attachments[0]);
    glDrawBuffers(1, attachments);
    m_SSAOPassFBO.Unbind();
#pragma endregion
#pragma region lensFlare
    m_LensFlareFBO.Bind();
    m_Passes[LENS_FLARE] =
        std::make_shared<Texture>(m_Width / 2, m_Height / 2, Texture::RGB);
    m_LensFlareFBO.AttachTexture(m_Passes[LENS_FLARE]->GetTextureID(),
                                 attachments[0]);
    m_Passes[VOLUME] =
        std::make_shared<Texture>(m_Width / 2, m_Height / 2, Texture::RGBA);
    m_LensFlareFBO.AttachTexture(m_Passes[VOLUME]->GetTextureID(),
                                 attachments[1]);
    glDrawBuffers(2, attachments);
    m_LensFlareFBO.Unbind();
#pragma endregion

#pragma region lighting pass texture
    m_LightPassFBO.Bind();
    m_Passes[LIGHTING] =
        std::make_shared<Texture>(m_Width, m_Height, Texture::RGBA);
    m_LightPassFBO.AttachTexture(m_Passes[LIGHTING]->GetTextureID(),
                                 attachments[0]);
    glDrawBuffers(1, attachments);

    // GLuint rbo1;

    // glCreateRenderbuffers(1, &rbo1);
    // glNamedRenderbufferStorage(rbo1, GL_DEPTH24_STENCIL8, m_Width, m_Height);

    // glNamedFramebufferRenderbuffer(m_LightPassFBO.GetBufferID(),
    //                                GL_DEPTH_STENCIL_ATTACHMENT,
    //                                GL_RENDERBUFFER, rbo1);

    m_LightPassFBO.Unbind();
#pragma endregion
}

void Pipeline::Render(const Scene &scene) {
    ShadowPass(scene);
    BasePass(scene);
    if (m_UseSSAO)
        SSAOPass(scene);
    LightPass(scene);
    LensFlarePass(scene);
    CompositorPass(scene);
}

void Pipeline::ShadowPass(const Scene &scene) {
    MVP lightMVP;
    ModelData modelInfo;
    LightData lightInfo;

    Renderer::EnableDepthTest();
    Renderer::EnableCullFace();

    // every light
    for (const auto &light : scene.GetLights()) {
        if (!light->GetCastShadow())
            continue;

        if (light->GetType() == Light::DIRECTION) {
            m_DirectionLightShadow.Bind();
            lightMVP.viewProjection = light->GetLightOrth();

        } else if (light->GetType() == Light::POINT ||
                   light->GetType() == Light::SPOT) {
            m_PointLightShadow.Bind();
        }

        lightInfo = light->GetLightData();

        glViewport(0, 0, light->GetTextureSize(), light->GetTextureSize());
        m_ShadowFBO.AttachTexture(light->GetShadowTexture()->GetTextureID(),
                                  GL_DEPTH_ATTACHMENT);
        m_ShadowFBO.Bind();
        Renderer::Clear();
        m_PointLightShadow.Validate();

        for (const auto &model : scene.GetModels()) {
            if (!model->GetCastShadows())
                continue;

            lightMVP.model = model->GetTransform().GetTransformMatrix();
            modelInfo = model->GetModelData();
            m_UBOs[0]->SetData(0, sizeof(MVP), &lightMVP);
            m_UBOs[1]->SetData(0, sizeof(ModelData), &modelInfo);
            // use first one to render shadow
            m_UBOs[2]->SetData(0, sizeof(LightData), &lightInfo);

            model->Draw();
        }

        m_ShadowFBO.Unbind();
        m_PointLightShadow.Unbind();
        glGenerateTextureMipmap(light->GetShadowTexture()->GetTextureID());
    }
}

void Pipeline::BasePass(const Scene &scene) {
    m_BasicPassFBO.Bind();
    m_Basic.Bind();

    glViewport(0, 0, m_Width, m_Height);

    Renderer::EnableCullFace();
    Renderer::EnableDepthTest();
    Renderer::Clear();

    MVP modelMVP;
    ModelData modelInfo;
    CameraData camData = scene.GetActiveCamera()->GetCameraData();

    m_Basic.Validate();

    for (const auto &model : scene.GetModels()) {
        if (!model->GetVisible())
            continue;
        if (model->GetUseAlbedoTexture() && model->GetAlbedoTexture())
            model->GetAlbedoTexture()->Bind(ALBEDO);
        if (model->GetUseEmissionTexture() && model->GetEmissionTexture())
            model->GetEmissionTexture()->Bind(EMISSION);
        if (model->GetUseNormalTexture() && model->GetNormalTexture())
            model->GetNormalTexture()->Bind(NORMAL);
        if (model->GetUseARMTexture() && model->GetARMTexture())
            model->GetARMTexture()->Bind(ARM);

        modelMVP.model = model->GetTransform().GetTransformMatrix();
        modelMVP.viewProjection = scene.GetActiveCamera()->GetViewProjection();

        modelInfo = model->GetModelData();
        m_UBOs[0]->SetData(0, sizeof(MVP), &modelMVP);
        m_UBOs[1]->SetData(0, sizeof(ModelData), &modelInfo);
        // m_UBOs[2]->SetData(0, sizeof(LightData) * m_MaxLightCount,
        // &lightInfo);
        m_UBOs[3]->SetData(0, sizeof(CameraData), &camData);
        model->Draw();
    }

    // avoid light mesh write depth texture
    glDepthMask(GL_FALSE);
    // mesh of light
    for (const auto &light : scene.GetLights()) {
        modelMVP.model = light->GetTransform().GetTransformMatrix();
        modelMVP.viewProjection = scene.GetActiveCamera()->GetViewProjection();

        modelInfo = light->GetModelData();
        m_UBOs[0]->SetData(0, sizeof(MVP), &modelMVP);
        m_UBOs[1]->SetData(0, sizeof(ModelData), &modelInfo);
        m_UBOs[3]->SetData(0, sizeof(CameraData), &camData);
        light->Draw();
    }

    glDepthMask(GL_TRUE);
    m_Basic.Unbind();
    m_BasicPassFBO.Unbind();
}

void Pipeline::SSAOPass(const Scene &scene) {
    m_SSAOPassFBO.Bind();
    m_SSAO.Bind();

    // half sample
    glViewport(0, 0, m_Width / 2, m_Height / 2);

    Renderer::DisableDepthTest();
    Renderer::EnableCullFace();
    Renderer::Clear();

    CameraData camData = scene.GetActiveCamera()->GetCameraData();

    m_SSAO.Validate();

    m_UBOs[3]->SetData(0, sizeof(CameraData), &camData);

    m_Passes[NORMAL]->Bind(NORMAL);
    m_Passes[DEPTH]->Bind(DEPTH);
    m_Passes[NOISE]->Bind(NOISE);

    m_Screen.Bind();
    // glTextureBarrier();
    Renderer::Draw(m_Screen.GetIndexBuffer()->GetCount());
    glGenerateTextureMipmap(m_Passes[SSAO]->GetTextureID());

    m_SSAO.Unbind();
    m_SSAOPassFBO.Unbind();
}

void Pipeline::LightPass(const Scene &scene) {
    m_LightPassFBO.Bind();
    m_Light.Bind();
    glViewport(0, 0, m_Width, m_Height);
    CameraData camData = scene.GetActiveCamera()->GetCameraData();
    const auto lights = scene.GetLights();

    for (int i = 0; i <= DEPTH; i++)
        m_Passes[i]->Bind(i);

    Renderer::DisableDepthTest(); // direct render texture no need depth
    Renderer::EnableCullFace();
    Renderer::Clear();

    m_Passes[ALBEDO]->Bind(ALBEDO);
    m_Passes[EMISSION]->Bind(EMISSION);
    m_Passes[NORMAL]->Bind(NORMAL);
    m_Passes[ARM]->Bind(ARM);
    m_Passes[POSITION]->Bind(POSITION);
    m_Passes[ID]->Bind(ID);
    m_Passes[DEPTH]->Bind(DEPTH);
    m_Passes[LIGHTING]->Bind(LIGHTING);
    m_Passes[SSAO]->Bind(SSAO);

    m_Light.Validate();

    for (const auto &light : lights) {

        if (light->GetCastShadow()) {
            if (light->GetType() == Light::POINT ||
                light->GetType() == Light::SPOT) {
                light->GetShadowTexture()->Bind(POINT_SHADOW);

            } else if (light->GetType() == Light::DIRECTION)
                light->GetShadowTexture()->Bind(DIRECTION_SHADOW);
        }

        if (light->GetUseColorTexture() && light->GetColorTexture())
            light->GetColorTexture()->Bind(REFLECT);

        LightData lightInfo = light->GetLightData();
        PipelineData pipelineInfo = GetPipelineData(scene);

        m_UBOs[2]->SetData(0, sizeof(LightData), &lightInfo);
        m_UBOs[3]->SetData(0, sizeof(CameraData), &camData);
        m_UBOs[4]->SetData(0, sizeof(PipelineData), &pipelineInfo);
        m_Screen.Bind();
        // make sure the texture have write before next draw
        glTextureBarrier();
        Renderer::Draw(m_Screen.GetIndexBuffer()->GetCount());
    }

    // glGenerateTextureMipmap(m_Passes[LIGHTING]->GetTextureID());
    m_Light.Unbind();
    m_LightPassFBO.Unbind();
}

void Pipeline::LensFlarePass(const Scene &scene) {
    m_LensFlareFBO.Bind();
    m_LensFlare.Bind();
    m_LensFlare.Validate();
    glViewport(0, 0, m_Width / 2, m_Height / 2);
    Renderer::DisableDepthTest();
    Renderer::EnableCullFace();
    Renderer::Clear();

    m_Passes[EMISSION]->Bind(EMISSION);
    m_Passes[DEPTH]->Bind(DEPTH);
    m_Passes[NOISE]->Bind(NOISE);

    m_Passes[VOLUME]->Bind(VOLUME);
    m_Passes[LENS_FLARE]->Bind(LENS_FLARE);

    m_Screen.Bind();
    CameraData camData = scene.GetActiveCamera()->GetCameraData();
    PipelineData pipelineInfo = GetPipelineData(scene);
    m_UBOs[3]->SetData(0, sizeof(CameraData), &camData);
    m_UBOs[4]->SetData(0, sizeof(PipelineData), &pipelineInfo);

    for (const auto &light : scene.GetLights()) {
        if (light->GetCastShadow()) {
            if (light->GetType() == Light::POINT ||
                light->GetType() == Light::SPOT) {
                light->GetShadowTexture()->Bind(POINT_SHADOW);

            } else if (light->GetType() == Light::DIRECTION)
                light->GetShadowTexture()->Bind(DIRECTION_SHADOW);
        }
        LightData lightInfo = light->GetLightData();
        m_UBOs[2]->SetData(0, sizeof(LightData), &lightInfo);

        glTextureBarrier();
        glFinish();
        Renderer::Draw(m_Screen.GetIndexBuffer()->GetCount());
    }
    glGenerateTextureMipmap(m_Passes[LENS_FLARE]->GetTextureID());
    glGenerateTextureMipmap(m_Passes[VOLUME]->GetTextureID());
    m_LensFlare.Unbind();
    m_LensFlareFBO.Unbind();
}
void Pipeline::CompositorPass(const Scene &scene) {
    Renderer::DisableDepthTest(); // direct render texture no need depth
    Renderer::EnableCullFace();
    if (m_UseToneMap)
        glEnable(GL_FRAMEBUFFER_SRGB);

    glViewport(0, 0, m_Width, m_Height);
    m_Compositor.Bind();

    m_Compositor.Validate();
    m_Passes[ALBEDO]->Bind(ALBEDO);
    m_Passes[EMISSION]->Bind(EMISSION);
    m_Passes[NORMAL]->Bind(NORMAL);
    m_Passes[ARM]->Bind(ARM);
    m_Passes[POSITION]->Bind(POSITION);
    m_Passes[ID]->Bind(ID);
    m_Passes[DEPTH]->Bind(DEPTH);
    m_Passes[SSAO]->Bind(SSAO);
    m_Passes[LIGHTING]->Bind(LIGHTING);
    m_Passes[VOLUME]->Bind(VOLUME);

    if (scene.GetEnvironmentMap())
        scene.GetEnvironmentMap()->Bind(REFLECT);

    m_Screen.Bind();
    PipelineData pipelineInfo = GetPipelineData(scene);
    m_UBOs[4]->SetData(0, sizeof(PipelineData), &pipelineInfo);
    Renderer::Draw(m_Screen.GetIndexBuffer()->GetCount());

    if (m_UseToneMap)
        glDisable(GL_FRAMEBUFFER_SRGB);

    m_Compositor.Unbind();
}

void Pipeline::SetWidth(int width) {
    m_Width = width;
    UpdatePass();
}

void Pipeline::SetHeight(int height) {
    m_Height = height;
    UpdatePass();
}

void Pipeline::UpdatePass() {
    for (const auto &pass : m_Passes) {
        if (pass.first == REFLECT || pass.first == POINT_SHADOW ||
            pass.first == DIRECTION_SHADOW)
            continue;

        pass.second->SetWidth(m_Width);
        pass.second->SetHeight(m_Height);
    }
}
void Pipeline::SavePass(Pass targetPass, const std::string &path) {
    if (m_Passes.find(targetPass) == m_Passes.end())
        throw std::runtime_error("not support texture to save as image");
    m_Passes[targetPass]->SaveTexture(path);
}

unsigned int Pipeline::GetIdByPosition(glm::vec2 pos) {

    GLubyte id = m_Passes[ID]->GetPixelColorByPosition(glm::vec2{
        pos.x, m_Height - pos.y})[3]; // OpenGL and GLFW have different Y axis
                                      // direction id is stored in alpha channel
    return static_cast<unsigned int>(id);
}
Pipeline::PipelineData Pipeline::GetPipelineData(const Scene &scene) {

    return PipelineData{
        scene.GetActiveSceneObjectID(),
        1.0f,
        1.0f,
        m_ActivePass,
        m_UseSSAO,
        m_UseOutline,
        m_UseHDRI,
        m_UseToneMap,
        m_VolumeColor,
        m_UseVolume,
        m_VolumeDensity,
        glm::vec3(),
    };
}
