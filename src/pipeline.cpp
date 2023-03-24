#include "pipeline.hpp"

#include "renderer.hpp"

Pipeline::Pipeline(int width, int height)
    : m_PointLightShadow("../assets/shaders/shadow.vert",
                         "../assets/shaders/shadow.geom",
                         "../assets/shaders/shadow.frag"),
      m_Basic("../assets/shaders/phong.vert",
              "../assets/shaders/deferred_pass.frag"),
      m_Light("../assets/shaders/frame_deferred.vert",
              "../assets/shaders/pipeline_light.frag"),
      m_Compositor("../assets/shaders/frame_screen.vert",
                   "../assets/shaders/frame_screen.frag"),
      m_Width(width), m_Height(height) {

    m_Basic.Bind();
    m_Basic.SetInt("albedoMap", ALBEDO);
    m_Basic.SetInt("emissionMap", EMISSION);
    m_Basic.SetInt("normalMap", NORMAL);
    m_Basic.SetInt("ARM", ARM);
    m_Basic.SetInt("reflectMap", REFLECT);

    m_Light.Bind();
    m_Light.SetInt("screenAlbedo", ALBEDO);
    m_Light.SetInt("screenEmission", EMISSION);
    m_Light.SetInt("screenNormal", NORMAL);
    m_Light.SetInt("screenARM", ARM);
    m_Light.SetInt("screenPosition", POSITION);
    m_Light.SetInt("screenDepth", DEPTH);

    m_Light.SetInt("screenLight", LIGHTING);
    m_Light.SetInt("screenVolume", VOLUME);

    m_Light.SetInt("reflectMap", REFLECT);

    m_Light.SetInt("pointShadowMap", POINT_SHADOW);
    m_Light.SetInt("directionShadowMap", DIRECTION_SHADOW);

    m_Compositor.Bind();
    m_Compositor.SetInt("screenAlbedo", ALBEDO);
    m_Compositor.SetInt("screenEmission", EMISSION);
    m_Compositor.SetInt("screenNormal", NORMAL);
    m_Compositor.SetInt("screenARM", ARM);
    m_Compositor.SetInt("screenPosition", POSITION);
    m_Compositor.SetInt("screenDepth", DEPTH);

    m_Compositor.SetInt("reflectMap", REFLECT);
    m_Compositor.SetInt("screenLight", LIGHTING);
    m_Compositor.SetInt("screenVolume", VOLUME);

    m_UBOs.push_back(std::make_shared<UniformBuffer>(sizeof(MVP), 0));
    m_UBOs.push_back(std::make_shared<UniformBuffer>(sizeof(ModelData), 1));
    m_UBOs.push_back(std::make_shared<UniformBuffer>(sizeof(LightData), 2));
    m_UBOs.push_back(std::make_shared<UniformBuffer>(sizeof(CameraData), 3));

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
                                  GL_COLOR_ATTACHMENT4};

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
    m_Passes[DEPTH] =
        std::make_shared<Texture>(m_Width, m_Height, Texture::DEPTH);
    m_BasicPassFBO.AttachTexture(m_Passes[DEPTH]->GetTextureID(),
                                 GL_DEPTH_ATTACHMENT);
    glDrawBuffers(5, attachments);
    m_BasicPassFBO.Unbind();
#pragma endregion

#pragma region lighting pass texture
    m_LightPassFBO.Bind();
    m_Passes[LIGHTING] =
        std::make_shared<Texture>(m_Width, m_Height, Texture::RGBA);
    m_LightPassFBO.AttachTexture(m_Passes[LIGHTING]->GetTextureID(),
                                 attachments[0]);
    m_Passes[VOLUME] =
        std::make_shared<Texture>(m_Width, m_Height, Texture::RGBA);
    m_LightPassFBO.AttachTexture(m_Passes[VOLUME]->GetTextureID(),
                                 attachments[1]);
    glDrawBuffers(2, attachments);

    GLuint rbo1;

    glCreateRenderbuffers(1, &rbo1);
    glNamedRenderbufferStorage(rbo1, GL_DEPTH24_STENCIL8, m_Width, m_Height);

    glNamedFramebufferRenderbuffer(m_LightPassFBO.GetBufferID(),
                                   GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER,
                                   rbo1);

    m_LightPassFBO.Unbind();
#pragma endregion
}

void Pipeline::Render(Scene scene) {
    ShadowPass(scene);
    BasePass(scene);
    LightPass(scene);
    CompositorPass();
}

void Pipeline::ShadowPass(Scene scene) {
    MVP lightMVP;
    ModelData modelInfo;

    std::vector<LightData> lightInfos;

    for (unsigned int i = 0; i < scene.GetLights().size(); i++) {
        std::shared_ptr<Light> light = scene.GetLights()[i];
        lightInfos.push_back(light->GetLightData());
    }

    Renderer::EnableDepthTest();
    Renderer::DisableCullFace();

    // every light
    for (unsigned int i = 0; i < scene.GetLights().size(); i++) {
        std::shared_ptr<Light> light = scene.GetLights()[i];
        if (!light->GetCastShadow())
            continue;

        if (light->GetType() == Light::POINT || light->GetType() == Light::SPOT)
            m_PointLightShadow.Bind();
        if (light->GetType() == Light::DIRECTION)
            m_PointLightShadow.Bind();

        glViewport(0, 0, light->GetTextureSize(), light->GetTextureSize());
        m_ShadowFBO.AttachTexture(light->GetShadowTexture()->GetTextureID(),
                                  GL_DEPTH_ATTACHMENT);
        m_ShadowFBO.Bind();
        Renderer::Clear();

        m_PointLightShadow.Validate();

        // not render light ball
        for (unsigned int j = 0; j < scene.GetModels().size() - 2; j++) {
            std::shared_ptr<Model> model = scene.GetModels()[j];
            if (!model->GetCastShadows())
                continue;

            lightMVP.model = model->GetTransform().GetTransformMatrix();
            modelInfo = model->GetModelData();
            m_UBOs[0]->SetData(0, sizeof(MVP), &lightMVP);
            m_UBOs[1]->SetData(0, sizeof(ModelData), &modelInfo);
            // use first one to render shadow
            m_UBOs[2]->SetData(0, sizeof(LightData), &lightInfos[i]);

            model->Draw();
        }

        m_ShadowFBO.Unbind();
        m_PointLightShadow.Unbind();
    }

    Renderer::EnableCullFace();
}

void Pipeline::BasePass(Scene scene) {
    m_BasicPassFBO.Bind();
    m_Basic.Bind();

    glViewport(0, 0, m_Width, m_Height);

    Renderer::EnableDepthTest();
    Renderer::Clear();

    MVP modelMVP;
    ModelData modelInfo;
    std::vector<LightData> lightInfos;
    CameraData camData = scene.GetActiveCamera()->GetCameraData();

    m_Basic.Validate();

    for (unsigned int i = 0; i < scene.GetModels().size(); i++) {
        std::shared_ptr<Model> model = scene.GetModels()[i];

        if (!model->GetVisible())
            continue;
        if (model->GetUseAlbedoTexture() && model->GetAlbedoTexture())
            model->GetAlbedoTexture()->Bind(ALBEDO);
        if (model->GetUseEmissionTexture() && model->GetEmissionTexture())
            model->GetEmissionTexture()->Bind(EMISSION);
        if (model->GetUseNormalTexture() && model->GetUseNormalTexture())
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

    m_Basic.Unbind();
    m_BasicPassFBO.Unbind();
}

void Pipeline::LightPass(Scene scene) {
    m_LightPassFBO.Bind();
    m_Light.Bind();
    glViewport(0, 0, m_Width, m_Height);
    CameraData camData = scene.GetActiveCamera()->GetCameraData();
    const auto lights = scene.GetLights();

    for (int i = 0; i <= DEPTH; i++)
        m_Passes[i]->Bind(i);

    // reflect pass need

    Renderer::DisableDepthTest(); // direct render texture no need depth
    Renderer::Clear();

    m_Passes[ALBEDO]->Bind(ALBEDO);
    m_Passes[EMISSION]->Bind(EMISSION);
    m_Passes[NORMAL]->Bind(NORMAL);
    m_Passes[ARM]->Bind(ARM);
    m_Passes[POSITION]->Bind(POSITION);
    m_Passes[DEPTH]->Bind(DEPTH);
    m_Passes[LIGHTING]->Bind(LIGHTING);
    m_Passes[VOLUME]->Bind(VOLUME);

    m_Light.Validate();

    for (unsigned int i = 0; i < lights.size(); i++) {
        std::shared_ptr<Light> light = lights[i];

        if (!light->GetCastShadow())
            continue;

        if (light->GetType() == Light::POINT || light->GetType() == Light::SPOT)
            light->GetShadowTexture()->Bind(POINT_SHADOW);
        else if (light->GetType() == Light::DIRECTION)
            light->GetShadowTexture()->Bind(DIRECTION_SHADOW);

        LightData lightInfo = light->GetLightData();
        m_UBOs[2]->SetData(0, sizeof(LightData), &lightInfo);
        m_UBOs[3]->SetData(0, sizeof(CameraData), &camData);
        // shader need to delete light type to select use cube for image 2d
        m_Screen.Bind();
        Renderer::Draw(m_Screen.GetIndexBuffer()->GetCount());
    }

    m_Light.Unbind();
    m_LightPassFBO.Unbind();
}

void Pipeline::CompositorPass() {
    Renderer::DisableDepthTest(); // direct render texture no need depth
    m_Compositor.Bind();

    m_Compositor.Validate();
    m_Passes[ALBEDO]->Bind(ALBEDO);
    m_Passes[EMISSION]->Bind(EMISSION);
    m_Passes[NORMAL]->Bind(NORMAL);
    m_Passes[ARM]->Bind(ARM);
    m_Passes[POSITION]->Bind(POSITION);
    m_Passes[DEPTH]->Bind(DEPTH);
    m_Passes[LIGHTING]->Bind(LIGHTING);
    m_Passes[VOLUME]->Bind(VOLUME);

    m_Screen.Bind();
    Renderer::Draw(m_Screen.GetIndexBuffer()->GetCount());

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
