#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "pch.hpp"

#include "transform.hpp"

// Up:      (0, 1, 0)
// Right:   (1, 0, 0)
// Front:   (0, 0, -1)

struct CameraData {
    TransformData transform;
    glm::mat4 projection;
    glm::mat4 view;
    float nearPlane;
    float farPlane;
    float aspectRatio;
    float FOV;
};
class Camera {
public:
    Camera(glm::vec3 position, float fov, float aspectRatio,
           float nearClip = 10.0f, float farClip = 1500.0f);

    void UpdateProjection();
    void UpdateView();

    void SetViewportSize(float width, float height);

    void Rotate(const float deltaX, const float deltaY);
    void Pan(float deltaX, float deltaY);
    void Zoom(float amount);

    glm::mat4 GetView() const { return m_View; };
    glm::mat4 GetViewProjection() const { return m_Projection * m_View; }

    CameraData GetCameraData();
    Transform &GetTransform() { return m_Transform; }
    const Transform &GetTransform() const { return m_Transform; }

private:
    float m_FOV; // measured in degrees
    float m_AspectRatio;
    float m_NearClip;
    float m_FarClip;

    float m_ViewportWidth = 1280, m_ViewportHeight = 720;

    glm::mat4 m_Projection;
    glm::mat4 m_View;

    Transform m_Transform;

    glm::vec3 m_Position;
    glm::vec3 m_Target;
};

#endif
