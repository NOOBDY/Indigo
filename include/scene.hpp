#ifndef SCENE_HPP
#define SCENE_HPP

#include "pch.hpp"

#include "camera.hpp"
#include "light.hpp"
#include "model.hpp"
#include "texture.hpp"

class Scene {
public:
    Scene(Camera defaultCamera);

    unsigned int AddCamera(Camera camera);
    unsigned int AddModel(Model model);
    unsigned int AddLight(Light light);

    void SetCamera(unsigned int id, Camera camera) {
        m_Cameras.at(id) = camera;
    }
    void SetModel(unsigned int id, Model model) { m_Models.at(id) = model; }
    void SetLight(unsigned int id, Light light) { m_Lights.at(id) = light; }

    void SetEnvironmentMap(std::shared_ptr<Texture> map) {
        m_EnvironmentMap = map;
    }

private:
    unsigned int m_ActiveCameraID;
    std::vector<Camera> m_Cameras;

    std::vector<Model> m_Models;
    std::vector<Light> m_Lights;

    std::shared_ptr<Texture> m_EnvironmentMap;
};

#endif