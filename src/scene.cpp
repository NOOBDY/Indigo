#include "scene.hpp"

Scene::Scene(Camera defaultCamera) : m_ActiveCameraID(0) {
    m_Cameras.push_back(defaultCamera);
}

unsigned int Scene::AddCamera(Camera camera) {
    m_Cameras.push_back(camera);
    return m_Cameras.size() - 1;
}

unsigned int Scene::AddModel(Model model) {
    m_Models.push_back(model);
    return m_Models.size() - 1;
}

unsigned int Scene::AddLight(Light light) {
    m_Lights.push_back(light);
    return m_Lights.size() - 1;
}
