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

    Program program("../assets/shaders/phong.vert",
                    "../assets/shaders/phong.frag");
    Program framebufferProgram("../assets/shaders/frame_screen.vert",
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

    glm::vec3 pos1(0, 0, 0);
    glm::vec3 rot1(180, 180, 180);
    glm::vec3 scale1(1, 1, 1);

    scene.push_back(
        Model{Importer::LoadFile("../assets/models/little_city/main.glb")});
    // end model 1

    // begin model 2
    Material matColor2 = {{0.0f, 0.8f, 0.8f}, 100.0f};

    glm::vec3 pos2(0, 0, 0);
    glm::vec3 rot2(180, 180, 180);
    glm::vec3 scale2(1, 1, 1);

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

    FrameBuffer fbo;
    fbo.Bind();

    // color buffer
    Texture renderSurface(SCREEN_WIDTH, SCREEN_HEIGHT);
    fbo.AttachTexture(renderSurface.GetTextureID(), GL_COLOR_ATTACHMENT0);
    Texture depthTexture(SCREEN_WIDTH, SCREEN_HEIGHT);
    fbo.AttachTexture(depthTexture.GetTextureID(), GL_DEPTH_ATTACHMENT);

    // render buffer
    GLuint rbo;

    glCreateRenderbuffers(1, &rbo);
    // glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glNamedRenderbufferStorage(rbo, GL_DEPTH24_STENCIL8, SCREEN_WIDTH,
                               SCREEN_HEIGHT);
    // glNamedRenderbufferStorage(rbo, GL_DEPTH_COMPONENT, 1280, 720);

    glNamedFramebufferRenderbuffer(
        fbo.GetBufferID(), GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    // when fbo is bind all render will storage and not display
    fbo.Unbind();

    float i = 0;

    // Small hack to put camera position into the shader
    // TODO: Find somewhere on the UBO to put this in
    GLint cameraUniform =
        glGetUniformLocation(program.GetProgramID(), "cameraPosition");

    do {
        glm::vec2 delta = window.GetCursorDelta();
        window.UpdateCursorPosition();

        fbo.Bind();

        Renderer::Clear();
        Renderer::EnableDepthTest();

        program.Bind();

        program.SetInt("texture1", 1);
        program.SetInt("texture2", 2);
        program.SetInt("texture3", 3);
        program.SetInt("texture4", 4);

        camera.GetTransform().SetPosition(
            camera.GetTransform().GetPosition() +
            10 * window.GetScrollOffset().y *
                glm::normalize(camera.GetTransform().GetPosition()));

        if (window.GetMouseButton(GLFW_MOUSE_BUTTON_RIGHT)) {
            camera.RotateByDelta(delta.x * -2 / window.GetWidth(),
                                 delta.y * -2 / window.GetHeight());
        }

        glm::vec3 pos = camera.GetTransform().GetPosition();
        glUniform3fv(cameraUniform, 1, &pos.x);

        camera.UpdateView();

        tex1.Bind(1);
        tex2.Bind(2);
        tex3.Bind(3);
        tex4.Bind(4);

        float tempValue = glm::sin(i += 0.1f);
        light1.m_Transform.SetPosition(glm::vec3(tempValue * 3, 2, 0));
        // light1.SetRadius(3 * glm::abs(tempValue));

        lightInfo[0] = light1.GetLightData();
        lightInfo[1] = light2.GetLightData();

        scene[0].VAO->Bind();

        scene[0].transform.SetPosition(pos1);
        scene[0].transform.SetRotation(rot1);
        scene[0].transform.SetScale(scale1);

        Matrices mat1;
        mat1.model = scene[0].transform.GetTransform();
        mat1.viewProjection = camera.GetViewProjection();
        matrices.SetData(0, sizeof(mat1), &mat1);
        materials.SetData(0, sizeof(Material), &matColor1);
        lights.SetData(0, sizeof(LightData) * LIGHT_NUMBER, &lightInfo);

        Renderer::Draw(scene[0].VAO->GetIndexBuffer()->GetCount());

        scene[1].VAO->Bind();

        scene[1].transform.SetPosition(pos2);
        scene[1].transform.SetRotation(rot2);
        scene[1].transform.SetScale(scale2);

        Matrices mat2;
        mat2.model = scene[1].transform.GetTransform();
        mat2.viewProjection = camera.GetViewProjection();
        matrices.SetData(0, sizeof(mat2), &mat2);
        materials.SetData(0, sizeof(Material), &matColor2);
        lights.SetData(0, sizeof(LightData) * LIGHT_NUMBER, &lightInfo);

        tex1.Bind(2);
        tex2.Bind(1);
        tex3.Bind(3);
        tex4.Bind(4);

        Renderer::Draw(scene[1].VAO->GetIndexBuffer()->GetCount());

        // frame buffer part
        fbo.Unbind();

        Renderer::DisableDepthTest(); // direct render texture no need depth
        framebufferProgram.Bind();
        framebufferProgram.SetInt("screenTexture", 0);
        // glBindVertexArray(quadVAO);
        planeVAO.Bind();
        glBindTexture(GL_TEXTURE_2D, renderSurface.GetTextureID());
        Renderer::Draw(planeVAO.GetIndexBuffer()->GetCount());
        // glDrawArrays(GL_TRIANGLES, 0, 6);
        // done frame buffer

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        float framerate = ImGui::GetIO().Framerate;

        ImGui::Begin("Debug Info");
        ImGui::Text("%.1f FPS", framerate);
        ImGui::Text("(%d, %d)", (int)delta.x, (int)delta.y);
        ImGui::Text("(%f, %f)", window.GetScrollOffset().x,
                    window.GetScrollOffset().y);
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
        window.PollEvents();
    } while (!window.ShouldClose());

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
