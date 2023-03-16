#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#include "pch.hpp"

#include "program.hpp"
#include "renderer.hpp"
#include "scene.hpp"
#include "frame_buffer.hpp"
#include "uniform_buffer.hpp"

class Pipeline {
    enum Pass {
        ALBEDO,
        EMISSION,
        NORMAL,
        ARM,
        POSITION,
        REFLECT,
        DEPTH,
        LIGHTING,
        VOLUME,
        SHADOW, // Must be last because cube map stuff
        PASS_NUMBER
    };
    struct MVP {
        glm::mat4 model;
        glm::mat4 viewProjection;
    };

public:
    Pipeline();

    void EnablePass(Pass pass);
    void Init(int maxLightCount);
    void Render(Scene scene);

private:
    void ShadowPass(Scene scene);
    void BasePass(Scene scene);
    void LightPass(Scene scene);
    void CompositePass();

private:
    std::shared_ptr<Program> m_PointLightShadow;
    std::shared_ptr<Program> m_Basic;
    std::shared_ptr<Program> m_Light;
    std::shared_ptr<Program> m_Compositor;
    FrameBuffer m_ShadowFBO;
    FrameBuffer m_BasicPassFBO;
    FrameBuffer m_LightPassFBO;
    FrameBuffer m_CompositeFBO;

    std::vector<std::shared_ptr<Texture>> m_LightDepths;
    std::vector<std::shared_ptr<UniformBuffer>> m_UBOs;
    std::vector<std::shared_ptr<Texture>> m_Passes;

    int m_Width;
    int m_Height;
    unsigned int m_MaxLightCount = 2;
};
#endif