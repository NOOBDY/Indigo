#include "scene.hpp"

#include <algorithm>

#include "scene_object.hpp"

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
    m_ActiveObjectID = id < SceneObject::GetIDCount() ? id : -1;
}

std::shared_ptr<SceneObject> Scene::GetActiveSceneObject() const {
    // I probably should've used `std::unordered_map` but oh well
    return m_ActiveObjectID != -1
               ? *std::find_if(m_SceneObjects.begin(), m_SceneObjects.end(),
                               [this](std::shared_ptr<SceneObject> obj) {
                                   return obj->GetID() == m_ActiveObjectID;
                               })
               : nullptr;
}
