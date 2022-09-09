#include "transform.hpp"

#include "log.hpp"

transform::transform(glm::vec3 position, glm::vec3 rotate, glm::vec3 scale,
                     glm::mat4 transform){};

glm::mat4 transform::scale_mat() {
    glm::mat4 scale_mat = glm::mat4(0.0f);
    scale_mat[0][0] = m_scale.x;
    scale_mat[1][1] = m_scale.y;
    scale_mat[2][2] = m_scale.z;
    scale_mat[3][3] = 1.0f;
    return scale_mat;
}
glm::mat4 transform::rotation_mat() {
    glm::mat4 rotation_mat = glm::rotate(glm::mat4(0.0), 30.f, m_rotation);
    return rotation_mat;
}
glm::mat4 transform::translation_mat() {
    glm::mat4 translation_mat = glm::mat4(0.0f);
    ;
    translation_mat[0][0] = 1.0f;
    translation_mat[1][1] = 1.0f;
    translation_mat[2][2] = 1.0f;
    translation_mat[3][3] = 1.0f;

    translation_mat[0][3] = m_position.x;
    translation_mat[1][3] = m_position.y;
    translation_mat[2][3] = m_position.z;

    return translation_mat;
}
void transform::update_transform(glm::vec3 position, glm::vec3 rotation,
                                 glm::vec3 scale, glm::mat4 set_transform) {
    if (set_transform[0][0] != NULL) {
        m_transform = set_transform;
        // position
        m_position.x = set_transform[0][3];
        m_position.y = set_transform[1][3];
        m_position.z = set_transform[2][3];
        // scale
        m_scale.x = set_transform[0][0];
        m_scale.y = set_transform[1][1];
        m_scale.z = set_transform[2][2];

    } else {
        if (position.x != NULL)
            m_position = position;
        if (scale.x != NULL)
            m_scale = scale;
        if (rotation.x != NULL)
            m_rotation = rotation;
        m_transform = scale_mat() * rotation_mat() * translation_mat();
    }
}