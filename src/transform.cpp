#include "transform.hpp"

#include <glm/gtx/matrix_decompose.hpp>

#include "log.hpp"

Transform::Transform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
    : m_Position(position), m_Rotation(rotation), m_Scale(scale) {}

void Transform::SetTransform(glm::mat4 transform) {
    m_Transform = transform;

    glm::quat rot;         // convert from quaternion to euler angles later
    glm::vec3 skew;        // unused
    glm::vec4 perspective; // unused

    glm::decompose(m_Transform, m_Scale, rot, m_Position, skew, perspective);

    m_Rotation = glm::eulerAngles(rot);
}

void Transform::UpdateMat() {
    glm::mat4 eye(1.f);

    m_Transform = glm::translate(eye, m_Position) *
                  glm::toMat4(glm::quat(glm::radians(m_Rotation))) *
                  glm::scale(eye, m_Scale);
}

glm::mat4 Transform::GetTransformMatrix() {
    UpdateMat();

    return m_Transform;
}

glm::vec3 Transform::GetDirection(glm::vec3 dir) const {
    glm::vec4 direction = glm::vec4(glm::vec3(dir), 1);
    glm::mat4 transform = glm::mat4(1.f);
    transform = glm::translate(transform, glm::vec3(0));
    transform = glm::toMat4(glm::quat(glm::radians(m_Rotation)));
    // transform = glm::scale(transform, glm::vec3(1)); // scale size to 1
    direction = transform * direction;
    return glm::vec3(direction);
}

glm::mat4 Transform::GetDirection4() const {
    glm::mat4 transform = glm::mat4(1.f);
    transform = glm::translate(transform, glm::vec3(0));
    transform = glm::toMat4(glm::quat(glm::radians(m_Rotation)));

    return transform;
}

TransformData Transform::GetTransformData() {
    UpdateMat();

    TransformData data = {
        m_Transform,    m_Position,
        0.0f, //
        m_Rotation,
        0.0f, //
        m_Scale,
        0.0f, //
        GetDirection(),
        0.0f, //
    };

    return data;
}
