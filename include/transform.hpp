#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include "pch.hpp"

struct transform_data {
    glm::mat4 transform;
};

class transform {
public:
    transform(glm::vec3 position = glm::vec3(0.0f),
              glm::vec3 rotation = glm::vec3(0.0f),
              glm::vec3 scale = glm::vec3(1.f),
              glm::mat4 set_transform = glm::mat4(NULL));

    void update_transform(glm::vec3 position = glm::vec3(NULL),
                          glm::vec3 rotation = glm::vec3(NULL),
                          glm::vec3 scale = glm::vec3(NULL),
                          glm::mat4 set_transform = glm::mat4(NULL));
    glm::mat4 Get_transform() const { return m_transform; };
    glm::vec3 Get_position() const { return m_position; };
    glm::vec3 Get_scale() const { return m_scale; };
    glm::vec3 Get_rotation() const { return m_rotation; };

    glm::mat4 rotaion_mat(glm::mat4 set_transform);
    glm::mat4 update_mat();
    void separate_mat(glm::mat4 set_transform);

    transform_data Get_transform_data() const { return packdata; };

private:
    glm::mat4 m_transform = glm::mat4(1.0f);
    glm::vec3 m_position = {0, 0, 0};
    glm::vec3 m_rotation = {0, 0, 0};
    glm::vec3 m_scale = {1, 1, 1};
    transform_data packdata;
};

#endif