#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "pch.hpp"

#include "transform.hpp"

// Up:      (0, 1, 0)
// Right:   (1, 0, 0)
// Front:   (0, 0, -1)

class Camera {
public:
    Camera(float fov, float aspectRatio, float nearClip = 10.0f,
           float farClip = 1500.0f);

    void UpdateProjection();
    void UpdateView();

    void SetViewportSize(float width, float height);

    void RotateByDelta(const float deltaX, const float deltaY);

    void Pan() {}

    glm::mat4 GetView() const { return m_View; };
    glm::mat4 GetViewProjection() const { return m_Projection * m_View; }

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
};

#endif