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

int main(int, char **) {
    Log::Init();

    Window window(SCREEN_WIDTH, SCREEN_HEIGHT);

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
    light2.SetPower(50);

    std::vector<Model> scene;

    // begin model 1
    Material matColor1 = {glm::vec3(0.8f, 0.5f, 0.0f), 100.0f};
    glm::vec3 uidata[10];

    uidata[0] = glm::vec3(1.35, 0, 0);
    uidata[1] = glm::vec3(180, 180, 180);
    uidata[2] = glm::vec3(1, 1, 1);
    scene.push_back(
        Model{Importer::LoadFile("../assets/models/little_city/main.glb")});
    // end model 1

    // begin model 2
    // Material matColor2 = {{0.0f, 0.8f, 0.8f}, 100.0f};

    uidata[3] = glm::vec3(-2, 0, 0);
    uidata[4] = glm::vec3(180, 180, 180);
    uidata[5] = glm::vec3(1);
    scene.push_back(
        Model{Importer::LoadFile("../assets/models/little_city/misc.glb")});
    // Importer::LoadFile("../assets/models/cube.obj");
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

    VertexArray planeVAO;

    planeVAO.AddVertexBuffer(
        std::make_shared<VertexBuffer>(quadVertices, 2 * sizeof(float)));

    planeVAO.AddVertexBuffer(
        std::make_shared<VertexBuffer>(quadUV, 2 * sizeof(float)));

    planeVAO.SetIndexBuffer(std::make_shared<IndexBuffer>(quadIndex));

    Texture tex1("../assets/textures/little_city/main_color.jpg");
    Texture tex2("../assets/textures/little_city/inter_view.jpg");
    Texture tex3("../assets/textures/T_Wall_Damaged_2x1_A_N.png");
    Texture tex4("../assets/textures/T_Wall_Damaged_2x1_A_N.png");

    FrameBuffer colorFbo;
    colorFbo.Bind();

    // color buffer
    Texture renderSurface(SCREEN_WIDTH, SCREEN_HEIGHT, Texture::COLOR);
    colorFbo.AttachTexture(renderSurface.GetTextureID(), GL_COLOR_ATTACHMENT0);
    Texture depthTexture(SCREEN_WIDTH, SCREEN_HEIGHT, Texture::DEPTH);
    colorFbo.AttachTexture(depthTexture.GetTextureID(), GL_DEPTH_ATTACHMENT);

    // render buffer
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
    int shadowSize = 1024 * .5;
    Texture lightDepthTexture(shadowSize, shadowSize, Texture::DEPTH,
                              Texture::CUBE);
    shadowFbo.AttachTexture(lightDepthTexture.GetTextureID(),
                            GL_DEPTH_ATTACHMENT);

    // Texture depthTexture(shadowSize, shadowSize, Texture::DEPTH,
    // Texture::CUBE); shadowFbo.AttachTexture(depthTexture.GetTextureID(),
    // GL_DEPTH_ATTACHMENT); Texture shadowTexture(shadowSize, shadowSize,
    // Texture::COLOR,
    //                       Texture::CUBE);
    // shadowFbo.AttachTexture(shadowTexture.GetTextureID(),
    // GL_COLOR_ATTACHMENT0);

    // float frameCount = 0;

    uidata[6] = glm::vec3(0, 0, 0);
    uidata[7] = glm::vec3(0, 0, 0);
    uidata[8] = glm::vec3(20);
    scene.push_back(Model{Importer::LoadFile("../assets/models/sphere.obj")});
    // Small hack to put camera position into the shader
    // TODO: Find somewhere on the UBO to put this in
    GLint cameraUniform =
        glGetUniformLocation(programColor.GetProgramID(), "cameraPosition");
    Matrices lightMat;

    do {
        // float tempValue = glm::sin(frameCount += 0.05f);
        // light1.m_Transform.Setition(glm::vec3(1, tempValue * 3, -3));
        light1.m_Transform.SetPosition(uidata[6]);
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
        glDisable(GL_CULL_FACE);

        for (int j = 0; j < scene.size() - 1; j++) {
            scene[j].VAO->Bind();
            scene[j].transform.SetPosition(uidata[3 * j]);
            scene[j].transform.SetRotation(uidata[3 * j + 1]);
            scene[j].transform.SetScale(uidata[3 * j + 2]);
            lightMat.model = scene[j].transform.GetTransform();
            matrices.SetData(0, sizeof(lightMat), &lightMat);
            lights.SetData(0, sizeof(LightData) * LIGHT_NUMBER, &lightInfo);
            Renderer::Draw(scene[j].VAO->GetIndexBuffer()->GetCount());
        }

        // color (phong shader)
        glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        // glEnable(GL_CULL_FACE);
        colorFbo.Bind();
        programColor.Bind();
        Renderer::Clear();

        glm::vec3 cameraPos = camera.GetTransform().GetPosition();
        glUniform3fv(cameraUniform, 1, &cameraPos.x);
        programColor.SetInt("texture1", 1);
        programColor.SetInt("texture2", 2);
        programColor.SetInt("texture3", 3);
        programColor.SetInt("texture4", 4);
        tex1.Bind(1);
        tex2.Bind(2);
        tex3.Bind(3);
        lightDepthTexture.Bind(4);
        // shadowTexture.Bind(4);
        // tex4.Bind(4);

        LOG_INFO(scene.size());
        for (int j = 0; j < scene.size(); j++) {
            scene[j].VAO->Bind();

            scene[j].transform.SetPosition(uidata[3 * j + 0]);
            scene[j].transform.SetRotation(uidata[3 * j + 1]);
            scene[j].transform.SetScale(uidata[3 * j + 2]);
            // if (j == 2)
            //     scene[j].transform.SetPosition(uidata[6]);

            Matrices mat1;
            mat1.model = scene[j].transform.GetTransform();
            mat1.viewProjection = camera.GetViewProjection();
            matrices.SetData(0, sizeof(mat1), &mat1);
            materials.SetData(0, sizeof(Material), &matColor1);
            lights.SetData(0, sizeof(LightData) * LIGHT_NUMBER, &lightInfo);
            Renderer::Draw(scene[0].VAO->GetIndexBuffer()->GetCount());
        }

        // frame buffer part
        colorFbo.Unbind();

        Renderer::DisableDepthTest(); // direct render texture no need depth
        programScreen.Bind();

        programScreen.SetInt("screenTexture", 0);
        programScreen.SetInt("depthTexture", 1);
        programScreen.SetInt("uvcheck", 2);
        renderSurface.Bind(0);
        depthTexture.Bind(1);
        // renderSurface.Bind(1);
        // shadowTexture.Bind(1);
        tex2.Bind(2);

        planeVAO.Bind();
        glBindTexture(GL_TEXTURE_2D, renderSurface.GetTextureID());
        Renderer::Draw(planeVAO.GetIndexBuffer()->GetCount());
        // done frame buffer

        if (window.GetMouseButton(GLFW_MOUSE_BUTTON_RIGHT)) {
            camera.RotateByDelta(delta.x * -2 / window.GetWidth(),
                                 delta.y * -2 / window.GetHeight());
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        float framerate = ImGui::GetIO().Framerate;

        ImGui::Begin("Debug Info");
        ImGui::Text("%.1f FPS", framerate);
        ImGui::Text("(%d, %d)", (int)delta.x, (int)delta.y);
        ImGui::Text("%f, %f, %f", scene[0].transform.GetPosition().x,
                    scene[0].transform.GetPosition().y,
                    scene[0].transform.GetPosition().z);
        ImGui::End();

        ImGui::Begin("Model 1");
        ImGui::SliderFloat3("Position", &uidata[0][0], -100, 100);
        ImGui::SliderFloat3("Rotation", &uidata[1][0], 0, 360);
        ImGui::SliderFloat3("Scale", &uidata[2][0], 0.1f, 5.0f);
        ImGui::End();

        ImGui::Begin("Model 2");
        ImGui::SliderFloat3("Position", &uidata[3][0], -200, 200);
        ImGui::SliderFloat3("Rotation", &uidata[4][0], 0, 360);
        ImGui::SliderFloat3("Scale", &uidata[5][0], 0.1f, 5.0f);
        ImGui::End();
        ImGui::Begin("light 3");
        ImGui::SliderFloat3("Position", &uidata[6][0], -200, 200);
        ImGui::SliderFloat3("Rotation", &uidata[7][0], 0, 360);
        ImGui::SliderFloat3("Scale", &uidata[8][0], 0.1f, 100.0f);
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // TODO: Figure this out and put it in `Window` class
        // glfwSwapInterval(0);
        glfwSwapBuffers(window.GetWindow());
        glfwPollEvents();
    } while (!window.ShouldClose());

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
