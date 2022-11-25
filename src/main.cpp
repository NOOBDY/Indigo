#include "pch.hpp"

#include "log.hpp"
#include "window.hpp"
#include "renderer.hpp"
#include "controller.hpp"
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

#define LIGHT_NUMBER 2

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

    Controller::InitGUI(window);

    Program programShadow("../assets/shaders/shadow.vert",
                          "../assets/shaders/shadow.geom",
                          "../assets/shaders/shadow.frag");
    Program programColor("../assets/shaders/phong.vert",
                         "../assets/shaders/phong.frag");
    Program programScreen("../assets/shaders/frame_screen.vert",
                          "../assets/shaders/frame_screen.frag");
    LightData lightInfo[LIGHT_NUMBER];
    UniformBuffer matrices(sizeof(Matrices), 0);
    UniformBuffer materials(sizeof(Material), 1);
    UniformBuffer lights(sizeof(LightData) * LIGHT_NUMBER, 2);

    Camera camera(45.0f, window.GetAspectRatio());

    Light light1(glm::vec3(1.0f));
    Light light2(glm::vec3(1.0f));
    light1.SetLightType(LightType::POINT);
    light2.SetLightType(LightType::DIRECTION);
    light2.SetPower(0.2f);
    // begin model 1
    Transform model1Trans;
    model1Trans.SetPosition(glm::vec3(2, 0, 0));

    Material matColor1 = {glm::vec3(0.8f, 0.5f, 0.0f), 100.0f};

    glm::vec3 pos1(1.35, 0, 0);
    glm::vec3 rot1(180, 180, 180);
    glm::vec3 scale1(1, 1, 1);

    VertexArray vao1 = Importer::LoadFile("../assets/models/wall.obj");
    // end model 1

    // begin model 2
    Transform model2Trans;
    model2Trans.SetPosition({2, 0, 0});
    Material matColor2 = {{0.0f, 0.8f, 0.8f}, 100.0f};

    glm::vec3 pos2(-2, 0, 0);
    glm::vec3 rot2(180, 180, 180);
    glm::vec3 scale2(.3);

    VertexArray vao2 = Importer::LoadFile("../assets/models/cube.obj");
    // end model 2

    // 2D plane for framebuffer
    std::vector<float> quadVertices = {
        -1.0f, 1.0f,  //
        -1.0f, -1.0f, //
        1.0f,  -1.0f, //
        1.0f,  1.0f,  //
    };
    std::vector<float> quadUV = {
        0.0f, 1.0f, //
        0.0f, 0.0f, //
        1.0f, 0.0f, //
        1.0f, 1.0f, //
    };
    std::vector<unsigned int> quadIndex = {
        0, 1, 2, //
        0, 2, 3, //
    };

    VertexArray planeVao;

    planeVao.AddVertexBuffer(
        std::make_shared<VertexBuffer>(quadVertices, 2 * sizeof(float)));

    planeVao.AddVertexBuffer(
        std::make_shared<VertexBuffer>(quadUV, 2 * sizeof(float)));

    planeVao.SetIndexBuffer(std::make_shared<IndexBuffer>(quadIndex));

    Texture tex1("../assets/textures/T_Wall_Damaged_2x1_A_BC.png");
    Texture tex2("../assets/textures/uv.png");
    Texture tex3("../assets/textures/T_Wall_Damaged_2x1_A_N.png");
    Texture tex4("../assets/textures/T_Wall_Damaged_2x1_A_N.png");

    FrameBuffer colorFbo;
    colorFbo.Bind();

    // color buffer
    Texture renderSurface(1280, 720, Texture::COLOR);
    colorFbo.AttachTexture(renderSurface.GetTextureID(), GL_COLOR_ATTACHMENT0);

    // render buffer
    GLuint rbo;

    glCreateRenderbuffers(1, &rbo);
    // glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glNamedRenderbufferStorage(rbo, GL_DEPTH24_STENCIL8, 1280, 720);
    // glNamedRenderbufferStorage(rbo, GL_DEPTH_COMPONENT, 1280, 720);

    glNamedFramebufferRenderbuffer(colorFbo.GetBufferID(),
                                   GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER,
                                   rbo);

    // when colorFbo is bind all render will storage and not display
    colorFbo.Unbind();
    FrameBuffer shadowFbo;
    shadowFbo.Bind();
    int shadowSize = 1024 * 4;
    int screenWidth = 1280;
    int screenHeight = 720;

    Texture depthTexture(shadowSize, shadowSize, Texture::DEPTH, Texture::CUBE);
    shadowFbo.AttachTexture(depthTexture.GetTextureID(), GL_DEPTH_ATTACHMENT);
    Texture shadowTexture(shadowSize, shadowSize, Texture::COLOR,
                          Texture::CUBE);
    shadowFbo.AttachTexture(shadowTexture.GetTextureID(), GL_COLOR_ATTACHMENT0);

    // shadow mat
    Matrices lightMat;
    glm::mat4 tt[6];
    LOG_INFO(sizeof(LightData));
    float i = 0;

    glm::vec3 pos = camera.GetPosition();
    VertexArray vao3 = Importer::LoadFile("../assets/models/sphere.obj");

    GLint cameraUniform =
        glGetUniformLocation(programColor.GetProgramID(), "cameraPosition");

    do {
        float tempValue = glm::sin(i += 0.05f);
        // light1.m_Transform.SetPosition(glm::vec3(1, tempValue * 3, -3));
        light1.m_Transform.SetPosition(pos2);
        // light1.SetRadius(3 * glm::abs(tempValue));
        lightInfo[0] = light1.GetLightData();
        lightInfo[1] = light2.GetLightData();
        glm::vec2 delta = window.GetCursorDelta();
        window.UpdateCursorPosition();

        // shadow

        // make sure render size is same as texture
        glViewport(0, 0, shadowSize, shadowSize);
        shadowFbo.Bind();
        programShadow.Bind();
        Renderer::Clear();
        Renderer::EnableDepthTest();

        // vao1 shadow
        vao1.Bind();
        lightMat.model = model1Trans.GetTransform();
        matrices.SetData(0, sizeof(lightMat), &lightMat);
        lights.SetData(0, sizeof(LightData) * LIGHT_NUMBER, &lightInfo);
        Renderer::Draw(vao1.GetIndexBuffer()->GetCount());

        // vao2 shadow
        vao2.Bind();
        model2Trans.SetPosition(glm::vec3(-1, 0, 0));
        model2Trans.SetRotation(rot2);
        model2Trans.SetScale(scale2);
        lightMat.model = model2Trans.GetTransform();
        matrices.SetData(0, sizeof(lightMat), &lightMat);
        lights.SetData(0, sizeof(LightData) * LIGHT_NUMBER, &lightInfo);
        Renderer::Draw(vao2.GetIndexBuffer()->GetCount());
        shadowFbo.Unbind();

        // color (phong shader)
        glViewport(0, 0, screenWidth, screenHeight);
        colorFbo.Bind();
        programColor.Bind();
        Renderer::Clear();

        glUniform3fv(cameraUniform, 1, &pos.x);

        tex1.Bind(1);
        tex2.Bind(2);
        tex3.Bind(3);
        shadowTexture.Bind(4);
        // tex4.Bind(4);

        programColor.SetInt("texture1", 1);
        programColor.SetInt("texture2", 2);
        programColor.SetInt("texture3", 3);
        programColor.SetInt("texture4", 4);

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

        vao2.Bind();

        model2Trans.SetPosition(glm::vec3(-1, 0, 0));
        // model2Trans.SetPosition(pos2);
        model2Trans.SetRotation(rot2);
        model2Trans.SetScale(scale2);

        Matrices mat2;
        mat2.model = model2Trans.GetTransform();
        mat2.viewProjection = camera.GetViewProjection();
        matrices.SetData(0, sizeof(mat2), &mat2);
        materials.SetData(0, sizeof(Material), &matColor2);
        lights.SetData(0, sizeof(LightData) * LIGHT_NUMBER, &lightInfo);

        tex1.Bind(2);
        tex2.Bind(1);
        tex3.Bind(3);
        tex4.Bind(4);

        Renderer::Draw(vao2.GetIndexBuffer()->GetCount());
        vao3.Bind();
        model2Trans.SetScale(glm::vec3(0.3));
        model2Trans.SetPosition(pos2);

        mat2.model = model2Trans.GetTransform();
        mat2.viewProjection = camera.GetViewProjection();
        matrices.SetData(0, sizeof(mat2), &mat2);
        lights.SetData(0, sizeof(LightData) * LIGHT_NUMBER, &lightInfo);
        Renderer::Draw(vao3.GetIndexBuffer()->GetCount());

        // frame buffer part
        colorFbo.Unbind();

        Renderer::DisableDepthTest(); // direct render texture no need depth
        programScreen.Bind();

        programScreen.SetInt("screenTexture", 0);
        programScreen.SetInt("depthTexture", 1);
        programScreen.SetInt("uvcheck", 2);
        renderSurface.Bind(0);
        // renderSurface.Bind(1);
        // depthTexture.Bind(1);
        shadowTexture.Bind(1);
        tex2.Bind(2);

        planeVao.Bind();
        Renderer::Draw(planeVao.GetIndexBuffer()->GetCount());
        // done frame buffer

        if (window.GetMouseButton(GLFW_MOUSE_BUTTON_LEFT)) {
            glm::mat4 cameraMat =
                glm::rotate(glm::mat4(1.0f), delta.x * -2 / window.GetWidth(),
                            glm::vec3(0, 1, 0)) *
                glm::rotate(glm::mat4(1.0f), delta.y * -2 / window.GetWidth(),
                            glm::vec3(1, 0, 0)) *
                glm::translate(glm::mat4(1.0f), pos);

            pos = glm::vec3(cameraMat[3]);
        }

        camera.SetPosition(pos);
        camera.SetDirection(pos * -1.0f);
        camera.UpdateView();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        float framerate = ImGui::GetIO().Framerate;

        ImGui::Begin("Debug Info");
        ImGui::Text("%.1f FPS", framerate);
        ImGui::Text("(%d, %d)", (int)delta.x, (int)delta.y);
        ImGui::Text("%f", 1.0 / framerate);
        ImGui::End();

        ImGui::Begin("Model 1");
        ImGui::SliderFloat3("Position", &pos1[0], -3, 3);
        ImGui::SliderFloat3("Rotation", &rot1[0], 0, 360);
        ImGui::SliderFloat3("Scale", &scale1[0], 0.1f, 5.0f);
        ImGui::End();

        ImGui::Begin("Model 2");
        ImGui::SliderFloat3("Position", &pos2[0], -3, 3);
        ImGui::SliderFloat3("Rotation", &rot2[0], 0, 360);
        ImGui::SliderFloat3("Scale", &scale2[0], 0.1f, 5.0f);
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // TODO: Figure this out and put it in `Window` class
        glfwSwapBuffers(window.GetWindow());
        glfwPollEvents();
    } while (!window.ShouldClose());

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
