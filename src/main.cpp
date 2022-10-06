#include "pch.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "log.hpp"
#include "window.hpp"
#include "renderer.hpp"
#include "program.hpp"
#include "camera.hpp"
#include "importer.hpp"
#include "vertex_array.hpp"
#include "vertex_buffer.hpp"
#include "index_buffer.hpp"
#include "uniform_buffer.hpp"
#include "texture.hpp"
#include "transform.hpp"
#include "light.hpp"

#pragma pack(16) // std140 layout pads by multiple of 16
struct Matrices {
    glm::mat4 model;
    glm::mat4 viewProjection;
};

struct Material {
    glm::vec3 baseColor;
    float maxShine;
};

int main(int, char **) {
    Log::Init();

    Window window;

    Renderer::Init();
    Renderer::ClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    IMGUI_CHECKVERSION();
    LOG_INFO("ImGui Version: {}", IMGUI_VERSION);
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.IniFilename = "../assets/imgui.ini";

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window.GetWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 460");

    Program program("../assets/shaders/phong.vert",
                    "../assets/shaders/phong.frag");

#define LIGHT_NUMBER 2
    LightData lightInfo[LIGHT_NUMBER];
    UniformBuffer matrices(sizeof(Matrices), 0);
    UniformBuffer materials(sizeof(Material), 1);
    UniformBuffer lights(sizeof(LightData) * LIGHT_NUMBER, 2);

    Camera camera(45.0f, window.GetAspectRatio());

    Light light1(glm::vec3(1.0f));
    Light light2(glm::vec3(1.0f));
    light1.SetLightType(LightType::POINT);
    light2.SetLightType(LightType::DIRECTION);
    // begin model 1
    Transform model1Trans;
    model1Trans.SetPosition(glm::vec3(2, 0, 0));

    Material matColor1 = {glm::vec3(0.8f, 0.5f, 0.0f), 100.0f};

    glm::vec3 pos1(2, 0, 0);
    glm::vec3 rot1(180, 180, 180);
    glm::vec3 scale1(1, 1, 1);

    Importer obj1("../assets/donut.obj");

    VertexArray vao1;

    vao1.AddVertexBuffer(
        std::make_shared<VertexBuffer>(obj1.GetVertices(), 3 * sizeof(float)));

    vao1.AddVertexBuffer(
        std::make_shared<VertexBuffer>(obj1.GetUVs(), 2 * sizeof(float)));

    vao1.AddVertexBuffer(
        std::make_shared<VertexBuffer>(obj1.GetNormals(), 3 * sizeof(float)));

    vao1.SetIndexBuffer(std::make_shared<IndexBuffer>(obj1.GetIndices()));
    // end model 1

    // begin model 2
    Transform model2Trans;
    model2Trans.SetPosition({2, 0, 0});
    Material matColor2 = {{0.0f, 0.8f, 0.8f}, 100.0f};

    glm::vec3 pos2(-2, 0, 0);
    glm::vec3 rot2(180, 180, 180);
    glm::vec3 scale2(1, 1, 1);

    Importer obj2("../assets/suzanne.obj");

    VertexArray vao2;

    vao2.AddVertexBuffer(
        std::make_shared<VertexBuffer>(obj2.GetVertices(), 3 * sizeof(float)));

    vao2.AddVertexBuffer(
        std::make_shared<VertexBuffer>(obj2.GetUVs(), 2 * sizeof(float)));

    vao2.AddVertexBuffer(
        std::make_shared<VertexBuffer>(obj2.GetNormals(), 3 * sizeof(float)));

    vao2.SetIndexBuffer(std::make_shared<IndexBuffer>(obj2.GetIndices()));
    // end model 2

    Texture tex1("../assets/fabric.png");
    Texture tex2("../assets/uv.png");

    program.Bind();

    program.SetInt("texture1", 0);
    program.SetInt("texture2", 1);

    float i = 0;

    do {
        Renderer::Clear();

        float tempValue = glm::sin(i += 0.1f);
        light1.m_Transform.SetPosition(glm::vec3(tempValue * 3, 2, 0));
        light1.SetRadius(3 * glm::abs(tempValue));

        lightInfo[1] = light1.GetLightData();
        lightInfo[0] = light2.GetLightData();

        vao1.Bind();

        model1Trans.SetPosition(pos1);
        model1Trans.SetRotation(rot1);
        model1Trans.SetScale(scale1);

        Matrices mat1;
        mat1.model = model1Trans.GetTransform();
        mat1.viewProjection = camera.GetViewProjection();
        matrices.SetData(0, sizeof(mat1), &mat1);
        materials.SetData(0, sizeof(Material), &matColor1);
        lights.SetData(0, sizeof(LightData) * LIGHT_NUMBER, &lightInfo);

        tex1.Bind(0);
        tex2.Bind(1);

        Renderer::Draw(vao1.GetIndexBuffer()->GetCount());
        vao2.Bind();

        model2Trans.SetPosition(pos2);
        model2Trans.SetRotation(rot2);
        model2Trans.SetScale(scale2);

        Matrices mat2;
        mat2.model = model2Trans.GetTransform();
        mat2.viewProjection = camera.GetViewProjection();
        matrices.SetData(0, sizeof(mat2), &mat2);
        materials.SetData(0, sizeof(Material), &matColor2);
        lights.SetData(0, sizeof(LightData) * LIGHT_NUMBER, &lightInfo);

        tex2.Bind(0);
        tex1.Bind(1);

        Renderer::Draw(vao2.GetIndexBuffer()->GetCount());

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Framerate");
        ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
        ImGui::End();

        ImGui::Begin("Donut");
        ImGui::SliderFloat3("Position", &pos1[0], -3, 3);
        ImGui::SliderFloat3("Rotation", &rot1[0], 0, 360);
        ImGui::SliderFloat3("Scale", &scale1[0], 0.1f, 5.0f);
        ImGui::End();

        ImGui::Begin("Suzanne");
        ImGui::SliderFloat3("Position", &pos2[0], -3, 3);
        ImGui::SliderFloat3("Rotation", &rot2[0], 0, 360);
        ImGui::SliderFloat3("Scale", &scale2[0], 0.1f, 5.0f);
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window.GetWindow());
        glfwPollEvents();
    } while (!window.ShouldClose());

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
