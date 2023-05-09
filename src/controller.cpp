#include "controller.hpp"

#include "log.hpp"

namespace Controller {
void InitGUI(const Window &window) {
    IMGUI_CHECKVERSION();
    LOG_INFO("ImGui Version: {}", IMGUI_VERSION);
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    // io.IniFilename = "../assets/imgui.ini";
    io.ConfigWindowsMoveFromTitleBarOnly = true;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window.GetWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 450");
}

void TransformGUI(std::shared_ptr<SceneObject> object) {
    glm::vec3 position = object->GetTransform().GetPosition();
    glm::vec3 rotation = object->GetTransform().GetRotation();
    glm::vec3 scale = object->GetTransform().GetScale();

    ImGui::Begin("Transform");
    ImGui::SetWindowPos({10, 10});
    ImGui::SetWindowSize({270, 100});
    ImGui::DragFloat3("Position", &position[0], 1, 0, 0, "%.1f");
    ImGui::DragFloat3("Rotation", &rotation[0], 1, 0, 360, "%.1f");
    ImGui::DragFloat3("Scale", &scale[0], 0.1f, 0, 0, "%.1f");
    ImGui::End();

    object->SetTransform(Transform(position, rotation, scale));
}

void ModelAttributeGUI(std::shared_ptr<Model> model) {
    auto meshColor = model->GetAlbedoColor();
    auto hasAlbedo = model->GetAlbedoTexture() != nullptr;
    auto useAlbedo = model->GetUseAlbedoTexture() && hasAlbedo;

    auto hasNormal = model->GetNormalTexture() != nullptr;
    auto useNormal = model->GetUseNormalTexture() && hasNormal;

    auto hasEmission = model->GetEmissionTexture() != nullptr;
    auto useEmission = model->GetUseEmissionTexture() && hasEmission;

    auto hasARM = model->GetARMTexture() != nullptr;
    auto useARM = model->GetUseARMTexture() && hasARM;
    float AO = model->GetAO();
    float roughness = model->GetRoughness();
    float metallic = model->GetMetallic();

    auto castShadow = model->GetCastShadows();
    auto visible = model->GetVisible();

    ImGui::Begin("Model Attributes");
    ImGui::SetWindowPos({10, 115});
    ImGui::SetWindowSize({270, 270});

    ImGui::BeginDisabled(useAlbedo);
    ImGui::ColorEdit3("Albedo Color", &meshColor[0]);
    ImGui::EndDisabled();

    ImGui::BeginDisabled(!hasAlbedo);
    ImGui::Checkbox("Albedo", &useAlbedo);
    ImGui::EndDisabled();
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) &&
        !hasAlbedo) {
        ImGui::SetTooltip("No albedo texture set");
    }

    ImGui::BeginDisabled(!hasNormal);
    ImGui::Checkbox("Normal", &useNormal);
    ImGui::EndDisabled();
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) &&
        !hasNormal) {
        ImGui::SetTooltip("No normal texture set");
    }

    ImGui::BeginDisabled(model->GetEmissionTexture() == nullptr);
    ImGui::Checkbox("Emission", &useEmission);
    ImGui::EndDisabled();
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) &&
        !hasEmission) {
        ImGui::SetTooltip("No emission texture set");
    }

    ImGui::BeginDisabled(model->GetARMTexture() == nullptr);
    ImGui::Checkbox("ARM", &useARM);
    ImGui::EndDisabled();
    ImGui::DragFloat("AO", &AO, 0.01, 0, 1, "%.2f");
    ImGui::DragFloat("roughness", &roughness, 0.01, 0, 1, "%.2f");
    ImGui::DragFloat("metallic", &metallic, 0.01, 0, 1, "%.2f");
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) && !hasARM) {
        ImGui::SetTooltip("No ARM texture set");
    }

    ImGui::Checkbox("Shadow", &castShadow);
    ImGui::Checkbox("Show/Hide", &visible);
    ImGui::End();

    model->SetAlbedoColor(meshColor);
    model->SetUseAlbedoTexture(useAlbedo);
    model->SetUseNormalTexture(useNormal);
    model->SetUseEmissionTexture(useEmission);
    model->SetUseARMTexture(useARM);
    model->SetAO(AO);
    model->SetRoughness(roughness);
    model->SetMetallic(metallic);
    model->SetCastShadows(castShadow);
    model->SetVisible(visible);
}

void LightAttributeGUI(std::shared_ptr<Light> light) {
    auto power = light->GetPower();
    auto radius = light->GetRadius();
    auto lightColor = light->GetColor();
    auto useColorTexture = light->GetUseColorTexture();
    // auto haveColorTexture = light->GetColorTexture() != nullptr;

    ImGui::Begin("Light Attributes");
    ImGui::SetWindowPos({10, 115});
    ImGui::SetWindowSize({270, 125});

    ImGui::BeginDisabled(useColorTexture);
    ImGui::ColorEdit3("Light Color", &lightColor[0]);
    ImGui::EndDisabled();

    ImGui::DragFloat("Power", &power, 0.05f, 0.0f, 10.0f, "%.2f");
    ImGui::DragFloat("Radius", &radius, 5, 1, 5000.0f, "%.1f");
    ImGui::End();

    light->SetPower(power);
    light->SetRadius(radius);
    light->SetLightColor(lightColor);
}

} // namespace Controller
