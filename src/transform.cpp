#include "transform.hpp"

#include "log.hpp"
#include "log.hpp"
transform::transform(glm::vec3 position, glm::vec3 rotate, glm::vec3 scale,
                     glm::mat4 transform) {
    update_transform(position, rotate, scale, transform);
};

glm::mat4 transform::update_mat() {
    glm::mat4 new_transform = glm::mat4(1.f);
    new_transform = glm::translate(new_transform, glm::vec3(0.f));
    // glm::vec3 temp = glm::vec3(180.f);
    // glm::vec3 temp = glm::mod(m_rotation, glm::vec3(360));

    m_rotation_vector = m_rotation / glm::vec3(180.f);
    // m_rotation_vector = glm::fract(m_rotation_vector);
    new_transform = glm::scale(new_transform, m_scale);
    new_transform = glm::rotate(new_transform, 360.0f, m_rotation_vector);
    new_transform = glm::translate(new_transform, m_position);
    return new_transform;
}
void transform::separate_mat(glm::mat4 set_transform) {
    m_transform = set_transform;
    // position
    m_position.x = set_transform[0][3];
    m_position.y = set_transform[1][3];
    m_position.z = set_transform[2][3];
    // scale
    m_scale.x = set_transform[0][0];
    m_scale.y = set_transform[1][1];
    m_scale.z = set_transform[2][2];

    // rotation is bitch form
    // https://www.reddit.com/r/opengl/comments/sih6lc/4x4_matrix_to_position_rotation_and_scale/
    const glm::vec3 left =
        glm::normalize(glm::vec3(set_transform[0])); // Normalized left axis
    const glm::vec3 up =
        glm::normalize(glm::vec3(set_transform[1])); // Normalized up axis
    const glm::vec3 forward =
        glm::normalize(glm::vec3(set_transform[2])); // Normalized forward axis

    // Obtain the "unscaled" transform matrix
    glm::mat4 m(0.0f);
    m[0][0] = left.x;
    m[0][1] = left.y;
    m[0][2] = left.z;

    m[1][0] = up.x;
    m[1][1] = up.y;
    m[1][2] = up.z;

    m[2][0] = forward.x;
    m[2][1] = forward.y;
    m[2][2] = forward.z;

    glm::vec3 rot;
    rot.x = atan2f(m[1][2], m[2][2]);
    rot.y = atan2f(-m[0][2], sqrtf(m[1][2] * m[1][2] + m[2][2] * m[2][2]));
    rot.z = atan2f(m[0][1], m[0][0]);
    rot = glm::degrees(rot); // Convert to degrees, or you could multiply it by
                             // (180.f / 3.14159265358979323846f)
    m_rotation = rot;
    m_rotation_vector = (m_rotation - glm::vec3(180.f)) / glm::vec3(180.f);
}
glm::mat4 transform::scale_mat() {
    glm::mat4 scale_mat = glm::mat4(0.0f);
    scale_mat[0][0] = m_scale.x;
    scale_mat[1][1] = m_scale.y;
    scale_mat[2][2] = m_scale.z;
    scale_mat[3][3] = 1.0f;
    return scale_mat;
}
void transform::update_transform(glm::vec3 position, glm::vec3 rotation,
                                 glm::vec3 scale, glm::mat4 set_transform) {

    if (set_transform != glm::mat4(NULL)) {
        packdata.transform = m_transform;

    } else {
        if (position != glm::vec3(NULL))
            m_position = position;
        if (scale != glm::vec3(NULL))
            m_scale = scale;
        if (rotation != glm::vec3(NULL))
            m_rotation = rotation;
        m_transform = update_mat();
        packdata.transform = m_transform;
    }
}