#include "pipeline.hpp"

// #define LIGHT_NUMBER 2

Pipeline::Pipeline(){};

void Pipeline::Init(int maxLightCount) {
    m_maxLightCount = maxLightCount;
    m_Shadow = std::make_shared<Program>("../assets/shaders/shadow.vert",
                                         "../assets/shaders/shadow.geom",
                                         "../assets/shaders/shadow.frag");

    m_Basic = std::make_shared<Program>("../assets/shaders/phong.vert",
                                        "../assets/shaders/deferred_pass.frag");
    m_Basic->Bind();
    m_Basic->SetInt("albedoMap", ALBEDO);
    m_Basic->SetInt("normalMap", NORMAL);
    m_Basic->SetInt("emissionMap", EMISSION);
    m_Basic->SetInt("reflectMap", REFLECT);
    m_Basic->SetInt("ARM", ARM);

    m_Light =
        std::make_shared<Program>("../assets/shaders/frame_deferred.vert",
                                  "../assets/shaders/deferred_light.frag");
    m_Light->Bind();
    m_Light->SetInt("screenAlbedo", ALBEDO);
    m_Light->SetInt("screenNormal", NORMAL);
    m_Light->SetInt("screenPosition", POSITION);
    m_Light->SetInt("screenEmission", EMISSION);
    m_Light->SetInt("reflectMap", REFLECT);
    m_Light->SetInt("screenARM", ARM);
    m_Light->SetInt("screenDepth", DEPTH);

    for (int i = 0; i < m_maxLightCount; i++) {
        m_Basic->Bind();
        m_Basic->SetInt("shadowMap[" + std::to_string(i) + "]", SHADOW + i);
        m_Light->Bind();
        m_Light->SetInt("shadowMap[" + std::to_string(i) + "]", SHADOW + i);
    }

    m_Compositor =
        std::make_shared<Program>("../assets/shaders/frame_screen.vert",
                                  "../assets/shaders/frame_screen.frag");
    m_Compositor->Bind();
    m_Compositor->SetInt("screenAlbedo", ALBEDO);
    m_Compositor->SetInt("screenNormal", NORMAL);
    m_Compositor->SetInt("screenPosition", POSITION);
    m_Compositor->SetInt("screenEmission", EMISSION);
    m_Compositor->SetInt("reflectMap", REFLECT);
    m_Compositor->SetInt("screenARM", ARM);
    m_Compositor->SetInt("screenLight", LIGHTING);
    m_Compositor->SetInt("screenVolume", VOLUME);
    m_Compositor->SetInt("screenDepth", DEPTH);
}