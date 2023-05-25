#include "camera.hpp"

#include "log.hpp"

Camera::Camera(float fov, float aspectRatio, float nearClip, float farClip)
    : m_FOV(fov), m_AspectRatio(aspectRatio), m_NearClip(nearClip),
      m_FarClip(farClip), m_Position({0, 500, 500}), m_Target({0, 0, 0}) {

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
    // glm::lookAt(m_Position, glm::vec3(0.0f), glm::vec3(0, 1, 0));
}

void Camera::SetViewportSize(float width, float height) {
    m_ViewportWidth = width;
    m_ViewportHeight = height;

    m_AspectRatio = width / height;

    UpdateProjection();
}

void Camera::Reset() {
    m_Position = {0, 500, 500};
    m_Target = {0, 0, 0};
}

/**
 * TODO: Refactor this in the future
 *
 * I'm very sorry for this abomination
 *
 * This solution hard codes a transformation matrix that rotates the
 * current direction by 90 degrees as the vertical rotational axis
 */
void Camera::Rotate(const float deltaX, const float deltaY) {

    // Since the camera will always point at (0, 0, 0), normalizing the
    // position vector can be used as the direction
    glm::vec3 normalizedVec = glm::normalize(m_Transform.GetPosition());

    // This is sprinkles a bit of magic called "linear algebra"
    // Y-Axis Rotational Matrix Format
    // [[ cos(x) 0 sin(x)]
    //  [   0    1   0   ]
    //  [-sin(x) 0 cos(x)]]
    glm::mat3 rotMat({
        {0, 0, 1},
        {0, 1, 0},
        {-1, 0, 0},
    });

    // I think C/C++ applies operations from the back or something so
    // Ax=b needs to be written as `b = x * A`
    glm::vec3 rightVec = normalizedVec * rotMat;

    // This offsets the camera and rotates it along (0, 0, 0)
    glm::mat4 cameraMat =
        glm::rotate(glm::mat4(1.0f), deltaX, glm::vec3(0, 1, 0)) *
        // This uses the rotated vertical axis
        glm::rotate(glm::mat4(1.0f), deltaY,
                    glm::vec3(rightVec.x, 0, rightVec.z)) *
        glm::translate(glm::mat4(1.0f), m_Position);

    m_Position = glm::vec3(cameraMat[3]);
}

void Camera::Zoom(float amount) {
    m_Position += amount * glm::normalize(m_Position - m_Target);
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
