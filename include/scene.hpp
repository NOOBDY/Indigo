#ifndef SCENE_HPP
#define SCENE_HPP

#include "pch.hpp"

#include "camera.hpp"
#include "light.hpp"
#include "model.hpp"
#include "texture.hpp"
#include "scene_object.hpp"

class Scene {
public:
    Scene(const std::shared_ptr<Camera> defaultCamera);

    void AddCamera(const std::shared_ptr<Camera> camera);
    void AddModel(const std::shared_ptr<Model> model);
    void AddLight(const std::shared_ptr<Light> light);

    std::shared_ptr<Camera> GetActiveCamera() const {
        return m_Cameras[m_ActiveCameraID];
    }

    std::vector<std::shared_ptr<SceneObject>> GetSceneObjects() const {
        return m_SceneObjects;
    }
    std::vector<std::shared_ptr<Model>> GetModels() const;
    std::vector<std::shared_ptr<Light>> GetLights() const;

    // ID should be the order of Model/Light added
    void SetActiveSceneObject(unsigned int id);
    /**
     * returns `nullptr` if there is no active object
     */
    std::shared_ptr<SceneObject> GetActiveSceneObject() const;
    int GetActiveSceneObjectID() const { return m_ActiveObjectID; }

    void SetEnvironmentMap(const std::shared_ptr<Texture> map) {
        m_EnvironmentMap = map;
    }
    std::shared_ptr<Texture> GetEnvironmentMap() const {
        return m_EnvironmentMap;
    }

private:
    unsigned int m_ActiveCameraID;
    std::vector<std::shared_ptr<Camera>> m_Cameras;

    int m_ActiveObjectID; // -1 for nothing active
    std::vector<std::shared_ptr<SceneObject>> m_SceneObjects;

    std::shared_ptr<Texture> m_EnvironmentMap;
};

#endif
