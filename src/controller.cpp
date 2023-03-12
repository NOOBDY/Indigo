#include "controller.hpp"

#include "log.hpp"

namespace Controller {
void InitGUI(Window &window) {
    IMGUI_CHECKVERSION();
    LOG_INFO("ImGui Version: {}", IMGUI_VERSION);
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.IniFilename = "../assets/imgui.ini";
    io.ConfigWindowsMoveFromTitleBarOnly = true;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window.GetWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 450");
}

TransformSlider::TransformSlider(const std::string label,
                                 const glm::vec3 position,
                                 const glm::vec3 rotation,
                                 const glm::vec3 scale)
    : m_Label(label + " Transform"), m_Position(position), m_Rotation(rotation),
      m_Scale(scale) {}

void TransformSlider::Update() {
    ImGui::Begin(m_Label.c_str());
    ImGui::SliderFloat3("Position", &m_Position[0], -300, 300);
    ImGui::SliderFloat3("Rotation", &m_Rotation[0], 0, 360);
    ImGui::SliderFloat3("Scale", &m_Scale[0], 0.1f, 5.0f);
    ImGui::End();
}
} // namespace Controller