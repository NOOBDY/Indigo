#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include "pch.hpp"

#include "window.hpp"
#include "transform.hpp"

namespace Controller {
void InitGUI(Window &window);

class TransformSlider {
public:
    TransformSlider(const std::string label, const glm::vec3 position,
                    const glm::vec3 rotation, const glm::vec3 scale);

    void Update();

    Transform GetTransform() {
        return Transform(m_Position, m_Rotation, m_Scale);
    }

private:
    std::string m_Label;

    glm::vec3 m_Position;
    glm::vec3 m_Rotation;
    glm::vec3 m_Scale;
};
} // namespace Controller

#endif