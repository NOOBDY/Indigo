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
        float detiaTime;
        int selectPass;
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

private:
    void ShadowPass(Scene scene);
    void BasePass(Scene scene);
    void LightPass(Scene scene);
    void CompositorPass(Scene scene);
    void ShadowPass(const Scene &scene);
    void BasePass(const Scene &scene);
    void LightPass(const Scene &scene);
    void CompositorPass(const Scene &scene);

private:
    void UpdatePass();
    Program m_PointLightShadow;
    Program m_DirectionLightShadow;
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
    int m_ActivePass;
};
#endif