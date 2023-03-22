#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#include "pch.hpp"

#include <unordered_map>

#include "program.hpp"
#include "scene.hpp"
#include "frame_buffer.hpp"
#include "uniform_buffer.hpp"

class Pipeline {
public:
    enum Pass {
        // basic
        ALBEDO,
        EMISSION,
        NORMAL,
        ARM,
        POSITION,
        ID,
        DEPTH,

        //
        REFLECT,

        // lighting
        LIGHTING,
        VOLUME,

        // shadow
        POINT_SHADOW, // Must be last because cube map stuff
        DIRECTION_SHADOW,

        SCREEN
    };
    struct MVP {
        glm::mat4 model;
        glm::mat4 viewProjection;
    };

    Pipeline(int width, int height);

    void Render(Scene scene);
    void SetWidth(int width);
    void SetHeight(int height);
    void Init();

private:
    void ShadowPass(Scene scene);
    void BasePass(Scene scene);
    void LightPass(Scene scene);
    void CompositorPass();

private:
    void UpdatePass();
    Program m_PointLightShadow;
    Program m_Basic;
    Program m_Light;
    Program m_Compositor;

    FrameBuffer m_ShadowFBO;
    FrameBuffer m_BasicPassFBO;
    FrameBuffer m_LightPassFBO;
    FrameBuffer m_CompositorFBO;

    std::vector<std::shared_ptr<UniformBuffer>> m_UBOs;
    std::unordered_map<int, std::shared_ptr<Texture>> m_Passes;

    VertexArray m_Screen;
    int m_Width;
    int m_Height;
};
#endif