#include "scene.hpp"

Scene::Scene(const std::shared_ptr<Camera> defaultCamera)
    : m_ActiveCameraID(0), m_ActiveObjectID(-1), m_EnvironmentMap(nullptr) {
    m_Cameras.push_back(defaultCamera);
}

void Scene::AddCamera(const std::shared_ptr<Camera> camera) {
    m_Cameras.push_back(camera);
}

void Scene::AddModel(const std::shared_ptr<Model> model) {
    m_SceneObjects.push_back(model);
}

void Scene::AddLight(const std::shared_ptr<Light> light) {
    m_SceneObjects.push_back(light);
}

std::vector<std::shared_ptr<Model>> Scene::GetModels() const {
    std::vector<std::shared_ptr<Model>> res;

    for (const auto &obj : m_SceneObjects) {
        if (obj->GetObjectType() == SceneObject::MODEL)
            res.push_back(std::dynamic_pointer_cast<Model>(obj));
    }

    return res;
}

std::vector<std::shared_ptr<Light>> Scene::GetLights() const {
    std::vector<std::shared_ptr<Light>> res;

    for (const auto &obj : m_SceneObjects) {
        if (obj->GetObjectType() == SceneObject::LIGHT)
            res.push_back(std::dynamic_pointer_cast<Light>(obj));
    }

    return res;
}

void Scene::SetActiveSceneObject(unsigned int id) {
    m_ActiveObjectID = id >= 0 && id < m_SceneObjects.size() ? id : -1;
}

std::shared_ptr<SceneObject> Scene::GetActiveSceneObject() const {
    return m_ActiveObjectID != -1 ? m_SceneObjects[m_ActiveObjectID] : nullptr;
}
