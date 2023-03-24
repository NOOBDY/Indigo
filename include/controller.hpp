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

    Transform GetTransform() const {
        return Transform(m_Position, m_Rotation, m_Scale);
    }

private:
    std::string m_Label;

    glm::vec3 m_Position;
    glm::vec3 m_Rotation;
    glm::vec3 m_Scale;
};

class LightSlider {
public:
    LightSlider(const std::string label, const float power, const float radius);

    void Update();

    float GetPower() const { return m_Power; }
    float GetRadius() const { return m_Radius; }

private:
    std::string m_Label;

    float m_Power;
    float m_Radius;
};
} // namespace Controller

#endif
