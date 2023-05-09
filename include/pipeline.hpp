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

        NOISE,
        SSAO,

        // lighting
        LIGHTING,
        VOLUME,

        // shadow
        POINT_SHADOW,
        DIRECTION_SHADOW,
        LUT,

        SCREEN
    };
    struct MVP {
        glm::mat4 model;
        glm::mat4 viewProjection;
    };
    struct PipelineData {
        int id;

        float time;
        float deltaTime;
        int selectPass;
        int useSSAO;
        glm::vec3 pad0;
        // glm::vec3 tem;
        // glm::vec2 mousePosition;
    };

    Pipeline(int width, int height);

    void Render(const Scene &scene);
    void SetWidth(int width);
    void SetHeight(int height);
    void SavePass(Pass targetPass, const std::string &path);
    unsigned int GetIdByPosition(glm::vec2 pos);
    void Init();
    PipelineData GetPipelineData(const Scene &scene);

private:
    void ShadowPass(const Scene &scene);
    void BasePass(const Scene &scene);
    void SSAOPass(const Scene &scene);
    void LightPass(const Scene &scene);
    void CompositorPass(const Scene &scene);

private:
    void UpdatePass();
    Program m_PointLightShadow;
    Program m_DirectionLightShadow;
    Program m_Basic;
    Program m_SSAO;
    Program m_Light;
    Program m_Compositor;

    FrameBuffer m_ShadowFBO;
    FrameBuffer m_BasicPassFBO;
    FrameBuffer m_SSAOPassFBO;
    FrameBuffer m_LightPassFBO;
    FrameBuffer m_CompositorFBO;

    std::vector<std::shared_ptr<UniformBuffer>> m_UBOs;
    std::unordered_map<int, std::shared_ptr<Texture>> m_Passes;

    VertexArray m_Screen;
    int m_Width;
    int m_Height;
    int m_ActivePass;

    bool m_UseSSAO;
};
#endif
