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

    vao1.SetIndexBuffer(std::make_shared<IndexBuffer>(obj1.GetIndices()));
    // const std::vector<std::vector<float>> test =
    //     vao1.CalculateTBN(obj1.GetVertices(), obj1.GetUVs(),
    //     obj1.GetIndices());
    // end model 1

    // begin model 2
    // Transform model2Trans;
    // model2Trans.SetPosition({2, 0, 0});
    // Material matColor2 = {{0.0f, 0.8f, 0.8f}, 100.0f};

    glm::vec3 pos2(-2, 0, 0);
    glm::vec3 rot2(180, 180, 180);
    glm::vec3 scale2(1, 1, 1);

    // Importer obj2("../assets/models/suzanne.obj");

    // VertexArray vao2;

    // vao2.AddVertexBuffer(
    //     std::make_shared<VertexBuffer>(obj2.GetVertices(), 3 *
    //     sizeof(float)));

    // vao2.AddVertexBuffer(
    //     std::make_shared<VertexBuffer>(obj2.GetUVs(), 2 * sizeof(float)));

    // vao2.AddVertexBuffer(
    //     std::make_shared<VertexBuffer>(obj2.GetNormals(), 3 *
    //     sizeof(float)));

    // vao2.SetIndexBuffer(std::make_shared<IndexBuffer>(obj2.GetIndices()));
    // end model 2
    float quadVertices[] = {// vertex attributes for a quad that fills the
                            // entire screen in Normalized Device Coordinates.
                            // positions   // texCoords
                            -1.0f, 1.0f, 0.0f, 1.0f,  -1.0f, -1.0f,
                            0.0f,  0.0f, 1.0f, -1.0f, 1.0f,  0.0f,

                            -1.0f, 1.0f, 0.0f, 1.0f,  1.0f,  -1.0f,
                            1.0f,  0.0f, 1.0f, 1.0f,  1.0f,  1.0f};
    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices,
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          (void *)(2 * sizeof(float)));

    Texture tex1("../assets/textures/T_Wall_Damaged_2x1_A_BC.png");
    Texture tex2("../assets/textures/uv.png");
    Texture tex3("../assets/textures/T_Wall_Damaged_2x1_A_N.png");
    Texture tex4("../assets/textures/T_Wall_Damaged_2x1_A_N.png");
    // vao1.Bind();

    FrameBuffer fbo;
    fbo.Bind();

    // color buffer
    // Texture renderSurface("../assets/textures/T_Wall_Damaged_2x1_A_N.png");
    Texture renderSurface(1280, 720);
    renderSurface.Bind();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1280, 720, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    fbo.AttachTexture(renderSurface.GetTextureID());
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           renderSurface.GetTextureID(), 0);

    // render buffer
    GLuint rbo;

    glCreateRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glNamedRenderbufferStorage(rbo, GL_DEPTH24_STENCIL8, 1280, 720);
    glNamedFramebufferRenderbuffer(
        fbo.GetBufferID(), GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    fbo.Unbind();

    float i = 0;

    do {
        fbo.Bind();
        // glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for
        glDisable(GL_DEPTH_TEST); // enable depth testing (is disabled for
        // rendering screen-space quad)
        program.Bind();
        tex1.Bind(1);
        tex2.Bind(2);
        tex3.Bind(3);
        tex4.Bind(4);

        // program1.Bind();
        program.SetInt("texture1", 1);
        program.SetInt("texture2", 2);
        program.SetInt("texture3", 3);
        program.SetInt("texture4", 4);
        tex1.Bind(1);
        tex2.Bind(2);
        tex3.Bind(3);
        tex4.Bind(4);

        Renderer::Clear();
        Renderer::ClearColor(1., 0, 0, 1);

        float tempValue = glm::sin(i += 0.1f);
        light1.m_Transform.SetPosition(glm::vec3(tempValue * 3, 2, 0));
        // light1.SetRadius(3 * glm::abs(tempValue));

        lightInfo[0] = light1.GetLightData();
        lightInfo[1] = light2.GetLightData();

        vao1.Bind();
        // glBindVertexArray(quadVAO);

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

        // Renderer::Clear();
        fbo.Unbind();
        // program.Unbind();
        program1.Bind();
        program1.SetInt("screenTexture", 0);
        // vao1.Bind();
        // Renderer::Draw(vao1.GetIndexBuffer()->GetCount());
        glBindVertexArray(quadVAO);
        glBindTexture(GL_TEXTURE_2D,
                      renderSurface.GetTextureID()); // use the color
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // glClear(GL_COLOR_BUFFER_BIT);

        /*
        fbo.Unbind();
        glViewport(0, 0, 1280, 720);
        Renderer::Clear();
        */

        /*
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

        tex1.Bind(1);
        tex2.Bind(2);
        tex3.Bind(3);
        tex4.Bind(4);

        Renderer::Draw(vao2.GetIndexBuffer()->GetCount());
        */

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
