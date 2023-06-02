#include "camera.hpp"

#include "log.hpp"

Camera::Camera(glm::vec3 position, float fov, float aspectRatio, float nearClip,
               float farClip)
    : m_FOV(fov), m_AspectRatio(aspectRatio), m_NearClip(nearClip),
      m_FarClip(farClip), m_Position(position), m_Target({0, 0, 0}) {

    UpdateProjection();
    UpdateView();
}

void Camera::UpdateProjection() {
    m_Projection = glm::perspective(glm::radians(m_FOV), m_AspectRatio,
                                    m_NearClip, m_FarClip);
}

/// @brief update look direction
void Camera::UpdateView() {
    m_Transform.SetPosition(m_Position);
    m_View = glm::lookAt(m_Position, m_Target, glm::vec3(0, 1, 0));
}

void Camera::SetViewportSize(float width, float height) {
    m_ViewportWidth = width;
    m_ViewportHeight = height;

    m_AspectRatio = width / height;

    UpdateProjection();
}

void Camera::Rotate(const float deltaX, const float deltaY) {
    const glm::vec3 right = glm::vec3(m_View[0][0], m_View[1][0], m_View[2][0]);
    const glm::vec3 up = glm::vec3(m_View[0][1], m_View[1][1], m_View[2][1]);

    const glm::mat4 temp =
        glm::translate(glm::mat4(1.0f), m_Position) *
        glm::rotate(glm::mat4(1.0f), glm::radians(deltaX), up) *
        glm::rotate(glm::mat4(1.0f), glm::radians(deltaY), right) *
        glm::translate(glm::mat4(1.0f), m_Target - m_Position);

    m_Target = glm::vec3(temp[3]);
}

void Camera::Zoom(float amount) {
    m_Position += amount * glm::normalize(m_Position - m_Target);
    m_Target += amount * glm::normalize(m_Position - m_Target);
}

void Camera::Pan(float deltaX, float deltaY) {
    glm::vec3 right = glm::vec3(m_View[0][0], m_View[1][0], m_View[2][0]);
    glm::vec3 up = glm::vec3(m_View[0][1], m_View[1][1], m_View[2][1]);

    m_Position += right * deltaX;
    m_Position += up * deltaY;
    m_Target += right * deltaX;
    m_Target += up * deltaY;
}

CameraData Camera::GetCameraData() {
    UpdateProjection();
    UpdateView();
    return CameraData{
        m_Transform.GetTransformData(),
        m_Projection,  //
        m_View,        //
        m_NearClip,    //
        m_FarClip,     //
        m_AspectRatio, //
        m_FOV,         //
    };
};
