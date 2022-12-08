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

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

#define SHADOW_SIZE 1024 / 2

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

struct Model {
    std::shared_ptr<VertexArray> VAO;
    Transform transform;
};

int main(int argc, char **argv) {
    Log::Init();

    if (argc > 1) {
        if (strncmp(argv[1], "-d", 2) == 0)
            Log::SetLevel(Log::DEBUG);

        if (strncmp(argv[1], "-t", 2) == 0)
            Log::SetLevel(Log::TRACE);
    }

    Window window(SCREEN_WIDTH, SCREEN_HEIGHT);

    Renderer::Init();
    Renderer::ClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    Controller::InitGUI(window);

    Program programShadow("../assets/shaders/shadow.vert",
                          "../assets/shaders/shadow.geom",
                          "../assets/shaders/shadow.frag");

    enum { ALBEDO, NORMAL, SHADOW };
    Program programColor("../assets/shaders/phong.vert",
                         "../assets/shaders/phong.frag");

    programColor.Bind();
    programColor.SetInt("albedoMap", ALBEDO);
    programColor.SetInt("normalMap", NORMAL);
    // programColor.SetInt("shadowMap", SHADOW);
    for (int i = 0; i < LIGHT_NUMBER; i++) {
        // LOG_INFO("shadowMap[" + std::to_string(i) + "]");
        programColor.SetInt("shadowMap[" + std::to_string(i) + "]", SHADOW + i);
    }

    // Small hack to put camera position into the shader
    // TODO: Find somewhere on the UBO to put this in
    GLint cameraUniform =
        glGetUniformLocation(programColor.GetProgramID(), "cameraPosition");

    Program programScreen("../assets/shaders/frame_screen.vert",
                          "../assets/shaders/frame_screen.frag");

    programScreen.Bind();
    programScreen.SetInt("screenTexture", 0);
    programScreen.SetInt("depthTexture", 1);
    programScreen.SetInt("depthTexture1", 2);
    for (int i = 0; i < LIGHT_NUMBER; i++) {
        // LOG_INFO("shadowMap[" + std::to_string(i) + "]");
        programScreen.SetInt("shadowMap[" + std::to_string(i) + "]", 3 + i);
    }
    // programScreen.SetInt("uvCheck", 2);

    LightData lightInfo[LIGHT_NUMBER];

    UniformBuffer matrices(sizeof(Matrices), 0);
    UniformBuffer materials(sizeof(Material), 1);
    UniformBuffer lights(sizeof(LightData) * LIGHT_NUMBER, 2);

    Camera camera(45.0f, window.GetAspectRatio());

    Light light1(Light::POINT, glm::vec3(1.0f));
    Light light2(Light::POINT, glm::vec3(1.0f));
    light2.SetPower(5);
    light2.SetRadius(400);

    std::vector<Model> scene;

    // begin model 1
    Material matColor1 = {glm::vec3(0.8f, 0.5f, 0.0f), 100.0f};
    float lightPower = 5;
    float lightRadius = 200;
    glm::mat3 uiData[3];

    uiData[0][0] = glm::vec3(0, 0, 0);
    uiData[0][1] = glm::vec3(180, 180, 180);
    uiData[0][2] = glm::vec3(1, 1, 1);
    scene.push_back(
        Model{Importer::LoadFile("../assets/models/little_city/main.glb")});
    // end model 1

    // begin model 2
    // Material matColor2 = {{0.0f, 0.8f, 0.8f}, 100.0f};

    uiData[1][0] = glm::vec3(0, 0, 0);
    uiData[1][1] = glm::vec3(180, 180, 180);
    uiData[1][2] = glm::vec3(1, 1, 1);
    scene.push_back(
        Model{Importer::LoadFile("../assets/models/little_city/interior.glb")});
    // end model 2

    // 2D plane for framebuffer
    VertexArray planeVAO;

    // vertices
    planeVAO.AddVertexBuffer(std::make_shared<VertexBuffer>(
        std::vector<float>{
            -1.0f, 1.0f,  //
            -1.0f, -1.0f, //
            1.0f, -1.0f,  //
            1.0f, 1.0f,   //
        },
        2 * sizeof(float)));

    // UV
    planeVAO.AddVertexBuffer(std::make_shared<VertexBuffer>(
        std::vector<float>{
            0.0f, 1.0f, //
            0.0f, 0.0f, //
            1.0f, 0.0f, //
            1.0f, 1.0f, //
        },
        2 * sizeof(float)));

    // Indices
    planeVAO.SetIndexBuffer(
        std::make_shared<IndexBuffer>(std::vector<unsigned int>{
            0, 1, 2, //
            0, 2, 3, //
        }));

    Texture texMainColor("../assets/textures/little_city/main_color.jpg");
    Texture texInterior("../assets/textures/little_city/interior.jpg");

    FrameBuffer colorFbo;
    colorFbo.Bind();

    // color buffer
    Texture renderSurface(SCREEN_WIDTH, SCREEN_HEIGHT, Texture::COLOR);
    colorFbo.AttachTexture(renderSurface.GetTextureID(), GL_COLOR_ATTACHMENT0);
    Texture depthTexture(SCREEN_WIDTH, SCREEN_HEIGHT, Texture::DEPTH);
    colorFbo.AttachTexture(depthTexture.GetTextureID(), GL_DEPTH_ATTACHMENT);

    // render buffer
    // TODO: Hide gl calls to somewhere else
    GLuint rbo;

    glCreateRenderbuffers(1, &rbo);
    glNamedRenderbufferStorage(rbo, GL_DEPTH24_STENCIL8, SCREEN_WIDTH,
                               SCREEN_HEIGHT);

    glNamedFramebufferRenderbuffer(colorFbo.GetBufferID(),
                                   GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER,
                                   rbo);

    // when colorFbo is bind all render will storage and not display
    colorFbo.Unbind();

    FrameBuffer shadowFbo;
    shadowFbo.Bind();
    std::vector<std::shared_ptr<Texture>> lightDepths;
    for (int i = 0; i < LIGHT_NUMBER; i++) {
        lightDepths.push_back(std::make_shared<Texture>(
            SHADOW_SIZE, SHADOW_SIZE, Texture::DEPTH, Texture::CUBE));
    }
    // Texture lightDepthTexture(SHADOW_SIZE, SHADOW_SIZE, Texture::DEPTH,
    //                           Texture::CUBE);
    // shadowFbo.AttachTexture(lightDepthTexture.GetTextureID(),
    //                         GL_DEPTH_ATTACHMENT);

    uiData[2][0] = glm::vec3(0, 0, 0);
    uiData[2][1] = glm::vec3(0, 0, 0);
    uiData[2][2] = glm::vec3(20);

    scene.push_back(Model{Importer::LoadFile("../assets/models/sphere.obj")});

    Matrices lightMat;

    do {
        light1.GetTransform().SetPosition(uiData[2][0]);
        light2.GetTransform().SetPosition(uiData[1][0]);
        light1.SetPower(lightPower);
        light1.SetRadius(lightRadius);
        lightInfo[0] = light1.GetLightData();
        lightInfo[1] = light2.GetLightData();
        glm::vec2 delta = window.GetCursorDelta();
        window.UpdateCursorPosition();

        // shadow

        // make sure render size is same as texture
        glViewport(0, 0, SHADOW_SIZE, SHADOW_SIZE);
        Renderer::EnableDepthTest();
        Renderer::DisableCullFace();

        for (int i = 0; i < lightDepths.size(); i++) {
            shadowFbo.AttachTexture(lightDepths[i]->GetTextureID(),
                                    GL_DEPTH_ATTACHMENT);
            shadowFbo.Bind();
            programShadow.Bind();
            Renderer::Clear();

            // not render light ball
            for (unsigned int j = 0; j < scene.size() - 1; j++) {
                scene[j].VAO->Bind();
                scene[j].transform.SetPosition(uiData[j][0]);
                scene[j].transform.SetRotation(uiData[j][1]);
                scene[j].transform.SetScale(uiData[j][2]);
                lightMat.model = scene[j].transform.GetTransform();
                matrices.SetData(0, sizeof(lightMat), &lightMat);
                // use first one to render shadow
                lights.SetData(0, sizeof(LightData), lightInfo + i);
                programShadow.Validate();
                Renderer::Draw(scene[j].VAO->GetIndexBuffer()->GetCount());
            }
            shadowFbo.Unbind();
        }

        // color (phong shader)
        glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        colorFbo.Bind();
        programColor.Bind();
        Renderer::Clear();

        camera.GetTransform().SetPosition(
            camera.GetTransform().GetPosition() +
            10 * window.GetScrollOffset().y *
                glm::normalize(camera.GetTransform().GetPosition()));

        if (window.GetMouseButton(GLFW_MOUSE_BUTTON_RIGHT)) {
            camera.RotateByDelta(delta.x * -2 / window.GetWidth(),
                                 delta.y * -2 / window.GetHeight());
        }

        glm::vec3 cameraPos = camera.GetTransform().GetPosition();
        glUniform3fv(cameraUniform, 1, &cameraPos.x);
        camera.UpdateView();

        texMainColor.Bind(ALBEDO);
        for (int i = 0; i < lightDepths.size(); i++) {
            lightDepths[i]->Bind(SHADOW + i);
        }
        // lightDepthTexture.Bind(SHADOW);

        for (unsigned int i = 0; i < scene.size(); i++) {
            scene[i].VAO->Bind();

            scene[i].transform.SetPosition(uiData[i][0]);
            scene[i].transform.SetRotation(uiData[i][1]);
            scene[i].transform.SetScale(uiData[i][2]);

            Matrices mat1;
            mat1.model = scene[i].transform.GetTransform();
            mat1.viewProjection = camera.GetViewProjection();
            matrices.SetData(0, sizeof(mat1), &mat1);
            materials.SetData(0, sizeof(Material), &matColor1);
            lights.SetData(0, sizeof(LightData) * LIGHT_NUMBER, &lightInfo);
            programColor.Validate();
            Renderer::Draw(scene[0].VAO->GetIndexBuffer()->GetCount());
        }

        // frame buffer part
        colorFbo.Unbind();

        Renderer::DisableDepthTest(); // direct render texture no need depth
        programScreen.Bind();
        renderSurface.Bind(0);
        lightDepths[0]->Bind(1);
        lightDepths[1]->Bind(2);
        lightDepths[0]->Bind(3);
        lightDepths[1]->Bind(4);
        // depthTexture.Bind(1);

        planeVAO.Bind();
        programScreen.Validate();
        Renderer::Draw(planeVAO.GetIndexBuffer()->GetCount());
        // done frame buffer

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        float framerate = ImGui::GetIO().Framerate;

        ImGui::Begin("Debug Info");
        ImGui::Text("%.1f FPS", framerate);
        ImGui::Text("(%d, %d)", (int)delta.x, (int)delta.y);
        ImGui::Text("(%d, %d)", (int)window.GetScrollOffset().x,
                    (int)window.GetScrollOffset().y);
        ImGui::End();

        ImGui::Begin("Model 1");
        ImGui::SliderFloat3("Position", &uiData[0][0][0], -300, 300);
        ImGui::SliderFloat3("Rotation", &uiData[0][1][0], 0, 360);
        ImGui::SliderFloat3("Scale", &uiData[0][2][0], 0.1f, 5.0f);
        ImGui::End();

        ImGui::Begin("Model 2");
        ImGui::SliderFloat3("Position", &uiData[1][0][0], -300, 300);
        ImGui::SliderFloat3("Rotation", &uiData[1][1][0], 0, 360);
        ImGui::SliderFloat3("Scale", &uiData[1][2][0], 0.1f, 5.0f);
        ImGui::End();

        ImGui::Begin("Light");
        ImGui::SliderFloat3("Position", &uiData[2][0][0], -300, 300);
        ImGui::SliderFloat3("Rotation", &uiData[2][1][0], 0, 360);
        ImGui::SliderFloat3("Scale", &uiData[2][2][0], 0.1f, 100.0f);
        ImGui::SliderFloat("Power", &lightPower, 0.1f, 10.0f);
        ImGui::SliderFloat("Radius", &lightRadius, 1.0f, 1000.0f);
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // TODO: Figure this out and put it in `Window` class
        // glfwSwapInterval(0);
        glfwSwapBuffers(window.GetWindow());
        window.PollEvents();
    } while (!window.ShouldClose());

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
