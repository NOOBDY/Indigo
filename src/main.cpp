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
#include "frame_buffer.hpp"
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
    Program program1("../assets/shaders/frame_screen.vert",
                     "../assets/shaders/frame_screen.frag");

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

    glm::vec3 pos1(1.35, 0, 0);
    glm::vec3 rot1(180, 180, 180);
    glm::vec3 scale1(1, 1, 1);

    Importer obj1("../assets/models/wall.obj");

    VertexArray vao1;

    vao1.AddVertexBuffer(
        std::make_shared<VertexBuffer>(obj1.GetVertices(), 3 * sizeof(float)));

    vao1.AddVertexBuffer(
        std::make_shared<VertexBuffer>(obj1.GetUVs(), 2 * sizeof(float)));

    vao1.AddVertexBuffer(
        std::make_shared<VertexBuffer>(obj1.GetNormals(), 3 * sizeof(float)));

    vao1.AddVertexBuffer(
        std::make_shared<VertexBuffer>(obj1.GetTangents(), 3 * sizeof(float)));

    vao1.AddVertexBuffer(std::make_shared<VertexBuffer>(obj1.GetBitangents(),
                                                        3 * sizeof(float)));

    vao1.SetIndexBuffer(std::make_shared<IndexBuffer>(obj1.GetIndices()));
    // end model 1

    // begin model 2
    Transform model2Trans;
    model2Trans.SetPosition({2, 0, 0});
    Material matColor2 = {{0.0f, 0.8f, 0.8f}, 100.0f};

    glm::vec3 pos2(-2, 0, 0);
    glm::vec3 rot2(180, 180, 180);
    glm::vec3 scale2(1, 1, 1);

    Importer obj2("../assets/models/suzanne.obj");

    VertexArray vao2;

    vao2.AddVertexBuffer(
        std::make_shared<VertexBuffer>(obj2.GetVertices(), 3 * sizeof(float)));

    vao2.AddVertexBuffer(
        std::make_shared<VertexBuffer>(obj2.GetUVs(), 2 * sizeof(float)));

    vao2.AddVertexBuffer(
        std::make_shared<VertexBuffer>(obj2.GetNormals(), 3 * sizeof(float)));

    vao2.AddVertexBuffer(
        std::make_shared<VertexBuffer>(obj2.GetTangents(), 3 * sizeof(float)));

    vao2.AddVertexBuffer(std::make_shared<VertexBuffer>(obj2.GetBitangents(),
                                                        3 * sizeof(float)));

    vao2.SetIndexBuffer(std::make_shared<IndexBuffer>(obj2.GetIndices()));
    // end model 2
    // 2D plane for framebuffer
    float quadVertices[] = {
        -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,
        1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f,
    };
    float quadUV[] = {0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
                      1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f};
    GLuint quadIndex[] = {0, 1, 2, 3, 4, 5};

    std::vector<float> quadVerticesVector(std::begin(quadVertices),
                                          std::end(quadVertices));
    std::vector<float> quadUvVector(std::begin(quadUV), std::end(quadUV));
    std::vector<GLuint> quadIndexVector(std::begin(quadIndex),
                                        std::end(quadIndex));

    VertexArray planeVao;

    planeVao.AddVertexBuffer(
        std::make_shared<VertexBuffer>(quadVerticesVector, 2 * sizeof(float)));

    planeVao.AddVertexBuffer(
        std::make_shared<VertexBuffer>(quadUvVector, 2 * sizeof(float)));

    planeVao.SetIndexBuffer(std::make_shared<IndexBuffer>(quadIndexVector));

    Texture tex1("../assets/textures/T_Wall_Damaged_2x1_A_BC.png");
    Texture tex2("../assets/textures/uv.png");
    Texture tex3("../assets/textures/T_Wall_Damaged_2x1_A_N.png");
    Texture tex4("../assets/textures/T_Wall_Damaged_2x1_A_N.png");

    FrameBuffer fbo;
    fbo.Bind();

    // color buffer
    Texture renderSurface(1280, 720);
    renderSurface.Bind();
    fbo.AttachTexture(renderSurface.GetTextureID(), GL_COLOR_ATTACHMENT0);
    Texture depthtTexture(1280, 720);
    depthtTexture.Bind();
    fbo.AttachTexture(depthtTexture.GetTextureID(), GL_DEPTH_ATTACHMENT);

    // render buffer
    GLuint rbo;

    glCreateRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glNamedRenderbufferStorage(rbo, GL_DEPTH24_STENCIL8, 1280, 720);
    // glNamedRenderbufferStorage(rbo, GL_DEPTH_COMPONENT, 1280, 720);

    glNamedFramebufferRenderbuffer(
        fbo.GetBufferID(), GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    // when fbo is bind all render will storage and not display
    fbo.Unbind();

    float i = 0;

    do {
        fbo.Bind();

        Renderer::Init();
        Renderer::Clear();
        Renderer::ClearColor(1., 0, 0, 1);
        program.Bind();
        tex1.BindUnit(1);
        tex2.BindUnit(2);
        tex3.BindUnit(3);
        tex4.BindUnit(4);

        program.SetInt("texture1", 1);
        program.SetInt("texture2", 2);
        program.SetInt("texture3", 3);
        program.SetInt("texture4", 4);

        float tempValue = glm::sin(i += 0.1f);
        light1.m_Transform.SetPosition(glm::vec3(tempValue * 3, 2, 0));
        // light1.SetRadius(3 * glm::abs(tempValue));

        lightInfo[0] = light1.GetLightData();
        lightInfo[1] = light2.GetLightData();

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

        Renderer::Draw(vao1.GetIndexBuffer()->GetCount());
        // glClear(GL_COLOR_BUFFER_BIT);

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

        tex1.BindUnit(2);
        tex2.BindUnit(1);
        tex3.BindUnit(3);
        tex4.BindUnit(4);

        Renderer::Draw(vao2.GetIndexBuffer()->GetCount());

        // frame buffer part
        fbo.Unbind();
        glDisable(GL_DEPTH_TEST); // direct render texture no need depth
        program1.Bind();
        program1.SetInt("screenTexture", 0);
        // glBindVertexArray(quadVAO);
        planeVao.Bind();
        glBindTexture(GL_TEXTURE_2D, renderSurface.GetTextureID());
        Renderer::Draw(planeVao.GetIndexBuffer()->GetCount());
        // glDrawArrays(GL_TRIANGLES, 0, 6);
        // done frame buffer

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
