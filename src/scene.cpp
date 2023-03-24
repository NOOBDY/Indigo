#include "scene.hpp"

Scene::Scene(const std::shared_ptr<Camera> defaultCamera)
    : m_ActiveCameraID(0) {
    m_Cameras.push_back(defaultCamera);
}

void Scene::Draw() const {
    for (const auto &model : m_Models) {
        model->Draw();
    }
}

void Scene::AddCamera(const std::shared_ptr<Camera> camera) {
    m_Cameras.push_back(camera);
}

void Scene::AddModel(const std::shared_ptr<Model> model) {
    m_Models.push_back(model);
}

void Scene::AddLight(const std::shared_ptr<Light> light) {
    m_Lights.push_back(light);
}
