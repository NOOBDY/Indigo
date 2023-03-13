#ifndef SCENE_HPP
#define SCENE_HPP

#include "pch.hpp"

#include "camera.hpp"
#include "light.hpp"
#include "model.hpp"
#include "texture.hpp"

class Scene {
public:
    Scene(const std::shared_ptr<Camera> defaultCamera);

    void Draw() const;

    void AddCamera(const std::shared_ptr<Camera> camera);
    void AddModel(const std::shared_ptr<Model> model);
    void AddLight(const std::shared_ptr<Light> light);
    std::vector<std::shared_ptr<Light>> GetLights() const { return m_Lights; };
    std::vector<std::shared_ptr<Model>> GetModel() const { return m_Models; };

    void SetEnvironmentMap(const std::shared_ptr<Texture> map) {
        m_EnvironmentMap = map;
    }

private:
    unsigned int m_ActiveCameraID;
    std::vector<std::shared_ptr<Camera>> m_Cameras;

    std::vector<std::shared_ptr<Model>> m_Models;
    std::vector<std::shared_ptr<Light>> m_Lights;

    std::shared_ptr<Texture> m_EnvironmentMap;
};

#endif