#include "camera.hpp"

#include "log.hpp"

Camera::Camera(float fov, float aspectRatio, float nearClip, float farClip)
    : m_FOV(fov), m_AspectRatio(aspectRatio), m_NearClip(nearClip),
      m_FarClip(farClip) {

    m_Transform.SetPosition({0, 500, 500});
    UpdateProjection();
    UpdateView();
}

void Camera::UpdateProjection() {
    m_Projection = glm::perspective(glm::radians(m_FOV), m_AspectRatio,
                                    m_NearClip, m_FarClip);
}

void Camera::UpdateView() {
    m_View = glm::lookAt(m_Transform.GetPosition(),
                         m_Transform.GetPosition() + m_Transform.GetRotation(),
                         glm::vec3(0, 1, 0));
    // glm::lookAt(m_Position, glm::vec3(0.0f), glm::vec3(0, 1, 0));
}

void Camera::SetViewportSize(float width, float height) {
    m_ViewportWidth = width;
    m_ViewportHeight = height;

    m_AspectRatio = width / height;

    UpdateProjection();
}