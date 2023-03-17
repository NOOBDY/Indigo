#include "pipeline.hpp"

Pipeline::Pipeline() {
    m_PointLightShadow =
        std::make_shared<Program>("../assets/shaders/shadow.vert", //
                                  "../assets/shaders/shadow.geom", //
                                  "../assets/shaders/shadow.frag");

    m_Basic = std::make_shared<Program>("../assets/shaders/phong.vert",
                                        "../assets/shaders/deferred_pass.frag");
    m_Basic->Bind();
    m_Basic->SetInt("albedoMap", ALBEDO);
    m_Basic->SetInt("emissionMap", EMISSION);
    m_Basic->SetInt("normalMap", NORMAL);
    m_Basic->SetInt("ARM", ARM);
    m_Basic->SetInt("reflectMap", REFLECT);

    m_Light =
        std::make_shared<Program>("../assets/shaders/frame_deferred.vert",
                                  "../assets/shaders/deferred_light.frag");
    m_Light->Bind();
    m_Light->SetInt("screenAlbedo", ALBEDO);
    m_Light->SetInt("screenEmission", EMISSION);
    m_Light->SetInt("screenNormal", NORMAL);
    m_Light->SetInt("screenARM", ARM);
    m_Light->SetInt("screenPosition", POSITION);
    m_Light->SetInt("screenDepth", DEPTH);
    m_Light->SetInt("reflectMap", REFLECT);

    // for (unsigned int i = 0; i < m_LightCount; i++) {
    //     // m_Basic->Bind();
    //     // m_Basic->SetInt("shadowMap[" + std::to_string(i) + "]", SHADOW +
    //     i); m_Light->Bind(); m_Light->SetInt("shadowMap[" + std::to_string(i)
    //     + "]", SHADOW + i);
    // }

    m_Compositor =
        std::make_shared<Program>("../assets/shaders/frame_screen.vert",
                                  "../assets/shaders/frame_screen.frag");
    m_Compositor->Bind();
    m_Compositor->SetInt("screenAlbedo", ALBEDO);
    m_Compositor->SetInt("screenEmission", EMISSION);
    m_Compositor->SetInt("screenNormal", NORMAL);
    m_Compositor->SetInt("screenARM", ARM);
    m_Compositor->SetInt("screenPosition", POSITION);
    m_Compositor->SetInt("screenDepth", DEPTH);

    m_Compositor->SetInt("reflectMap", REFLECT);
    m_Compositor->SetInt("screenLight", LIGHTING);
    m_Compositor->SetInt("screenVolume", VOLUME);

    m_UBOs.push_back(std::make_shared<UniformBuffer>(sizeof(MVP), 0));
    m_UBOs.push_back(std::make_shared<UniformBuffer>(sizeof(ModelData), 1));
    m_UBOs.push_back(std::make_shared<UniformBuffer>(sizeof(LightData), 2));
    m_UBOs.push_back(std::make_shared<UniformBuffer>(sizeof(CameraData), 3));
    VertexArray planeVAO;

    // vertices
    planeVAO.AddVertexBuffer(std::make_shared<VertexBuffer>(
        std::vector<float>{
            -1.0f, 1.0f,  //
            -1.0f, -1.0f, //
            1.0f, -1.0f,  //
            1.0f, 1.0f,   //
        },
        2 * sizeof(float)));

    // UV
    planeVAO.AddVertexBuffer(std::make_shared<VertexBuffer>(
        std::vector<float>{
            0.0f, 1.0f, //
            0.0f, 0.0f, //
            1.0f, 0.0f, //
            1.0f, 1.0f, //
        },
        2 * sizeof(float)));

    // Indices
    planeVAO.SetIndexBuffer(
        std::make_shared<IndexBuffer>(std::vector<unsigned int>{
            0, 1, 2, //
            0, 2, 3, //
        }));

    Model plane(std::make_shared<VertexArray>(planeVAO));
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
            m_PointLightShadow->Bind();
        if (light->GetType() == Light::DIRECTION)
            m_PointLightShadow->Bind();

        glViewport(0, 0, light->GetTextureSize(), light->GetTextureSize());
        m_ShadowFBO.AttachTexture(light->GetShadowTexture()->GetTextureID(),
                                  GL_DEPTH_ATTACHMENT);
        m_ShadowFBO.Bind();
        Renderer::Clear();

        // not render light ball
        for (unsigned int j = 0; j < scene.GetModels().size() - 2; j++) {
            std::shared_ptr<Model> model = scene.GetModels()[j];
            if (!model->GetCastShadows())
                continue;

            // scene[j].SetTransform(uiElements[j].GetTransform());
            lightMVP.model = model->GetTransform().GetTransformMatrix();
            modelInfo = model->GetModelData();
            m_UBOs[0]->SetData(0, sizeof(MVP), &lightMVP);
            m_UBOs[1]->SetData(0, sizeof(ModelData), &modelInfo);
            // use first one to render shadow
            m_UBOs[2]->SetData(0, sizeof(LightData), &lightInfos[i]);
            // m_PointLightShadow->Validate();

            model->Draw();
        }
        m_PointLightShadow->Unbind();
        m_ShadowFBO.Unbind();
    }

    Renderer::EnableCullFace();
}

void Pipeline::BasePass(Scene scene) {
    m_BasicPassFBO.Bind();
    m_Basic->Bind();
    glViewport(0, 0, m_Width, m_Height);
    Renderer::EnableDepthTest();
    Renderer::Clear();
    MVP modelMVP;
    ModelData modelInfo;
    std::vector<LightData> lightInfos;
    CameraData camData = scene.GetActiveCamera()->GetCameraData();
    for (unsigned int i = 0; i < scene.GetModels().size(); i++) {
        std::shared_ptr<Model> model = scene.GetModels()[i];
        if (!model->GetVisible())
            continue;
        modelMVP.model = model->GetTransform().GetTransformMatrix();
        modelMVP.viewProjection = scene.GetActiveCamera()->GetViewProjection();
        m_UBOs[0]->SetData(0, sizeof(MVP), &modelMVP);
        m_UBOs[1]->SetData(0, sizeof(ModelData), &modelInfo);
        // m_UBOs[2]->SetData(0, sizeof(LightData) * m_MaxLightCount,
        // &lightInfo);
        m_UBOs[3]->SetData(0, sizeof(CameraData), &camData);
        m_Basic->Validate();
        model->Draw();
    }
    m_Basic->Unbind();
    m_BasicPassFBO.Unbind();
}
void Pipeline::LightPass(Scene scene) {
    m_LightPassFBO.Bind();
    m_Light->Bind();
    glViewport(0, 0, m_Width, m_Height);
    CameraData camData = scene.GetActiveCamera()->GetCameraData();

    for (int i = 0; i <= DEPTH; i++) {
        m_Passes[i]->Bind(i);
    }
    // geo
    Renderer::DisableDepthTest(); // direct render texture no need depth
    Renderer::Clear();
    for (int i = 0; i < scene.GetLights().size(); i++) {
        std::shared_ptr<Light> light = scene.GetLights()[i];
        if (!light->GetCastShadow())
            continue;
        light->GetShadowTexture()->Bind(POINT_SHADOW);
        LightData lightInfo = light->GetLightData();
        m_UBOs[2]->SetData(0, sizeof(LightData), &lightInfo);
        m_UBOs[3]->SetData(0, sizeof(CameraData), &camData);
        m_Plane->Draw();
    }
    // programDeferredLight.Validate();
    m_LightPassFBO.Bind();
    m_Light->Bind();
    Renderer::EnableDepthTest();
}
void Pipeline::CompositorPass() {}
