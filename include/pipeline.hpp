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

        LENS_FLARE,

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
        int useOutline;
        int useHDRI;
        int useVolume;

        glm::vec3 volumeColor;
        float density;
        // glm::vec3 tem;
        // glm::vec2 mousePosition;
    };

    Pipeline(int width, int height);

    void Init();

    void Render(const Scene &scene);
    void SavePass(Pass targetPass, const std::string &path);

    void SetWidth(int width);
    void SetHeight(int height);
    void SetActivePass(Pipeline::Pass pass) {
        m_ActivePass = static_cast<int>(pass);
    }
    void SetUseSSAO(bool useSSAO) { m_UseSSAO = useSSAO; }
    void SetUseOutline(bool useOutline) { m_UseOutline = useOutline; }
    void SetUseHDRI(bool useHDRI) { m_UseHDRI = useHDRI; }
    void SetUseVolume(bool useVolume) { m_UseVolume = useVolume; }

    void SetVolumeDensity(float density) { m_VolumeDensity = density; }

    void SetUseVolumeColor(glm::vec3 volumeColor) {
        m_VolumeColor = volumeColor;
    }

    unsigned int GetIdByPosition(glm::vec2 pos);
    PipelineData GetPipelineData(const Scene &scene);
    Pipeline::Pass GetActivePass() const {
        return static_cast<Pipeline::Pass>(m_ActivePass);
    }
    bool GetUseSSAO() const { return m_UseSSAO; }
    bool GetUseOutline() const { return m_UseOutline; }
    bool GetUseHDRI() const { return m_UseHDRI; }
    bool GetUseVolume() const { return m_UseVolume; }

    float GetVolumeDensity() const { return m_VolumeDensity; }

    glm::vec3 GetUseVolumeColor() const { return m_VolumeColor; }

private:
    void ShadowPass(const Scene &scene);
    void BasePass(const Scene &scene);
    void SSAOPass(const Scene &scene);
    void LightPass(const Scene &scene);
    void LensFlarePass(const Scene &scene);
    void CompositorPass(const Scene &scene);

private:
    void UpdatePass();
    Program m_PointLightShadow;
    Program m_DirectionLightShadow;
    Program m_Basic;
    Program m_SSAO;
    Program m_Light;
    Program m_LensFlare;
    Program m_Compositor;

    FrameBuffer m_ShadowFBO;
    FrameBuffer m_BasicPassFBO;
    FrameBuffer m_SSAOPassFBO;
    FrameBuffer m_LightPassFBO;
    FrameBuffer m_LensFlareFBO;
    FrameBuffer m_CompositorFBO;

    std::vector<std::shared_ptr<UniformBuffer>> m_UBOs;
    std::unordered_map<int, std::shared_ptr<Texture>> m_Passes;

    VertexArray m_Screen;
    int m_Width;
    int m_Height;
    int m_ActivePass;

    bool m_UseSSAO;
    bool m_UseOutline;
    bool m_UseHDRI;
    bool m_UseVolume;

    float m_VolumeDensity;

    glm::vec3 m_VolumeColor;
    // glm::vec3 tem;
};
#endif
