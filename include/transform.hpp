#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include "pch.hpp"

struct TransformData {
    glm::mat4 transform;
    glm::vec3 position;
    float pad1;
    glm::vec3 rotation;
    float pad2;
    glm::vec3 scale;
    float pad3;
};

class Transform {
public:
    Transform() = default;
    Transform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);

    void SetTransform(glm::mat4 transform);

    void SetPosition(glm::vec3 position) { m_Position = position; }
    void SetRotation(glm::vec3 rotation) { m_Rotation = rotation; }
    void SetScale(glm::vec3 scale) { m_Scale = scale; }

    glm::mat4 GetTransform();
    glm::vec3 GetPosition() const { return m_Position; };
    glm::vec3 GetRotation() const { return m_Rotation; };
    glm::vec3 GetScale() const { return m_Scale; };

    TransformData GetTransformData();

private:
    glm::mat4 RotationMat(glm::mat4 transform);
    glm::mat4 UpdateMat();

private:
    glm::mat4 m_Transform = glm::mat4(1.0f);
    glm::vec3 m_Position = {0, 0, 0};
    glm::vec3 m_Rotation = {0, 0, 0};
    glm::vec3 m_Scale = {1, 1, 1};
};

#endif