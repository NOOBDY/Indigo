#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>

// Up:      (0, 1, 0)
// Right:   (1, 0, 0)
// Front:   (0, 0, -1)

class Camera {
public:
    Camera(float fov, float aspectRatio, float nearClip = 0.1f,
           float farClip = 100.0f);

    void UpdateProjection();
    void UpdateView();

    void SetViewportSize(float width, float height);

    glm::mat4 GetView() const { return m_View; };
    glm::mat4 GetViewProjection() const { return m_Projection * m_View; }

private:
    float m_FOV; // measured in degrees
    float m_AspectRatio;
    float m_NearClip;
    float m_FarClip;

    float m_ViewportWidth = 1280, m_ViewportHeight = 720;

    glm::mat4 m_Projection;
    glm::mat4 m_View;
    glm::vec3 m_Position = {0, 3, 4};
    glm::vec3 m_Target = {0, 0, 0};
};

#endif