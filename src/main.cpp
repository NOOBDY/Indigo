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
#include "model.hpp"
#include "light.hpp"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

#define SHADOW_SIZE 1024 * 2

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

    enum { ALBEDO, NORMAL,ARM, REFLECT,POSITION,DEPTH, SHADOW };
    Program programShadow("../assets/shaders/shadow.vert",
                          "../assets/shaders/shadow.geom",
                          "../assets/shaders/shadow.frag");

    Program programColor("../assets/shaders/phong.vert",
                         "../assets/shaders/phong.frag");
    Program programDeferredPass("../assets/shaders/phong.vert",
                         "../assets/shaders/deferredPass.frag");
    Program programDeferredLight("../assets/shaders/frame_deferred.vert",
                          "../assets/shaders/deferredLight.frag");
    Program programScreen("../assets/shaders/frame_screen.vert",
                          "../assets/shaders/frame_screen.frag");

    programScreen.Bind();
    programScreen.SetInt("screenTexture", 0);
    programScreen.SetInt("depthTexture", 1);
    programScreen.SetInt("uvCheck", 2);

    programDeferredLight.Bind();
    programDeferredLight.SetInt("screenAlbedo", ALBEDO);
    programDeferredLight.SetInt("screenNormal", NORMAL);
    programDeferredLight.SetInt("screenPosition", POSITION);
    programDeferredLight.SetInt("reflectMap", REFLECT);
    programDeferredLight.SetInt("screenARM", ARM);
    programDeferredLight.SetInt("screenDepth", DEPTH);

    programDeferredPass.Bind();
    programDeferredPass.SetInt("albedoMap", ALBEDO);
    programDeferredPass.SetInt("normalMap", NORMAL);
    programDeferredPass.SetInt("reflectMap", REFLECT);
    programDeferredPass.SetInt("ARM", ARM);
    // programDeferredPass.Unbind();

    // programColor.Bind();
    // programColor.SetInt("albedoMap", ALBEDO);
    // programColor.SetInt("normalMap", NORMAL);
    // programColor.SetInt("reflectMap", REFLECT);
    for (int i = 0; i < LIGHT_NUMBER; i++) {
        // programColor.SetInt("shadowMap[" + std::to_string(i) + "]", SHADOW + i);
        programDeferredPass.Bind();
        programDeferredPass.SetInt("shadowMap[" + std::to_string(i) + "]", SHADOW + i);
        programDeferredLight.Bind();
        programDeferredLight.SetInt("shadowMap[" + std::to_string(i) + "]", SHADOW + i);
    }

    // Small hack to put camera position into the shader
    // TODO: Find somewhere on the UBO to put this in
    // GLint cameraUniform =
    //     glGetUniformLocation(programColor.GetProgramID(), "cameraPosition");
    GLint cameraUniformDeferredPass =
        glGetUniformLocation(programDeferredPass.GetProgramID(), "cameraPosition");
    GLint cameraUniformDeferredLight=
        glGetUniformLocation(programDeferredLight.GetProgramID(), "cameraPosition");


    LightData lightInfo[LIGHT_NUMBER];

    UniformBuffer matrices(sizeof(Matrices), 0);
    UniformBuffer materials(sizeof(Material), 1);
    UniformBuffer lights(sizeof(LightData) * LIGHT_NUMBER, 2);
    UniformBuffer cameraUbo(sizeof(Camera) , 3);

    Camera camera(45.0f, window.GetAspectRatio());

    Light light1(Light::POINT, glm::vec3(1.0f));
    Light light2(Light::POINT, glm::vec3(1.0f));

    std::vector<Model> scene;

    // begin model 1
    Material matColor1 = {glm::vec3(0.8f, 0.5f, 0.0f), 100.0f};
    float lightPower[2];
    float lightRadius[2];
    glm::mat3 uiData[4];

    uiData[0][0] = glm::vec3(0, 0, 0);
    uiData[0][1] = glm::vec3(180, 180, 180);
    uiData[0][2] = glm::vec3(1, 1, 1);
    scene.push_back(
        Model(Importer::LoadFile("../assets/models/little_city/main.glb")));
    // end model 1

    // begin model 2
    // Material matColor2 = {{0.0f, 0.8f, 0.8f}, 100.0f};

    uiData[1][0] = glm::vec3(0, 0, 0);
    uiData[1][1] = glm::vec3(180, 180, 180);
    uiData[1][2] = glm::vec3(1, 1, 1);
    scene.push_back(
        Model(Importer::LoadFile("../assets/models/little_city/interior.glb")));
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

    Model plane(std::make_shared<VertexArray>(planeVAO));

    Texture texMainColor("../assets/textures/little_city/main_color.jpg");
    Texture texInterior("../assets/textures/little_city/interior.jpg");
    Texture reflectMap("../assets/textures/vestibule_2k.hdr");
    Texture wallNormalMap("../assets/textures/T_Wall_Damaged_2x1_A_N.png");


    FrameBuffer deferredFbo;
    deferredFbo.Bind();
    unsigned int attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 ,GL_COLOR_ATTACHMENT3 };

    // color buffer
    Texture screenAlbedo(SCREEN_WIDTH, SCREEN_HEIGHT, Texture::RGBA);
    deferredFbo.AttachTexture(screenAlbedo.GetTextureID(), attachments[0]);
    Texture screenNormal(SCREEN_WIDTH, SCREEN_HEIGHT, Texture::RGBA);
    deferredFbo.AttachTexture(screenNormal.GetTextureID(),  attachments[1]);
    Texture screenPosition(SCREEN_WIDTH, SCREEN_HEIGHT, Texture::RGBA);
    deferredFbo.AttachTexture(screenPosition.GetTextureID(), attachments[2]);
    Texture screenARM(SCREEN_WIDTH, SCREEN_HEIGHT, Texture::RGBA);
    deferredFbo.AttachTexture(screenARM.GetTextureID(), attachments[3]);
    Texture screenDepth(SCREEN_WIDTH, SCREEN_HEIGHT, Texture::DEPTH);
    deferredFbo.AttachTexture(screenDepth.GetTextureID(), GL_DEPTH_ATTACHMENT);
    glDrawBuffers(4, attachments);
    deferredFbo.Unbind();


    // FrameBuffer colorFbo;
    // colorFbo.Bind();

    // // color buffer
    // Texture renderSurface(SCREEN_WIDTH, SCREEN_HEIGHT, Texture::RGB);
    // colorFbo.AttachTexture(renderSurface.GetTextureID(), GL_COLOR_ATTACHMENT0);
    // Texture depthTexture(SCREEN_WIDTH, SCREEN_HEIGHT, Texture::DEPTH);
    // colorFbo.AttachTexture(depthTexture.GetTextureID(), GL_DEPTH_ATTACHMENT);

    // // render buffer
    // // TODO: Hide gl calls to somewhere else
    // GLuint rbo;

    // glCreateRenderbuffers(1, &rbo);
    // glNamedRenderbufferStorage(rbo, GL_DEPTH24_STENCIL8, SCREEN_WIDTH,
    //                            SCREEN_HEIGHT);

    // glNamedFramebufferRenderbuffer(colorFbo.GetBufferID(),
    //                                GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER,
    //                                rbo);

    // colorFbo.Unbind();

    FrameBuffer deferredLightFbo;
    deferredLightFbo.Bind();
    Texture screenLight(SCREEN_WIDTH, SCREEN_HEIGHT, Texture::RGBA);
    deferredLightFbo.AttachTexture(screenLight.GetTextureID(), attachments[0]);
    Texture screenVolume(SCREEN_WIDTH, SCREEN_HEIGHT, Texture::RGBA);
    deferredLightFbo.AttachTexture(screenVolume.GetTextureID(), attachments[1]);
    glDrawBuffers(2, attachments);
    deferredLightFbo.Unbind();
    //deferred
    GLuint rbo1;
    deferredLightFbo.Bind();

    glCreateRenderbuffers(1, &rbo1);
    glNamedRenderbufferStorage(rbo1, GL_DEPTH24_STENCIL8, SCREEN_WIDTH,
                               SCREEN_HEIGHT);

    glNamedFramebufferRenderbuffer(deferredLightFbo.GetBufferID(),
                                   GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER,
                                   rbo1);

    deferredLightFbo.Unbind();
    // when colorFbo is bind all render will storage and not display
    // colorFbo.Unbind();

    FrameBuffer shadowFbo;
    shadowFbo.Bind();
    std::vector<std::shared_ptr<Texture>> lightDepths;
    for (int i = 0; i < LIGHT_NUMBER; i++) {
        lightDepths.push_back(std::make_shared<Texture>(
            SHADOW_SIZE, SHADOW_SIZE, Texture::DEPTH, Texture::CUBE));
    }

    // light 1
    uiData[2][0] = glm::vec3(50, 100, 200);
    uiData[2][1] = glm::vec3(0, 0, 0);
    uiData[2][2] = glm::vec3(20);
    lightPower[0] = 1;
    lightRadius[0] = 500;
    // light 1
    uiData[3][0] = glm::vec3(-300, 300, 0);
    uiData[3][1] = glm::vec3(0, 0, 0);
    uiData[3][2] = glm::vec3(20);
    lightPower[1] = 2;
    lightRadius[1] = 500;

    scene.push_back(Model(Importer::LoadFile("../assets/models/sphere.obj")));
    scene.push_back(Model(Importer::LoadFile("../assets/models/sphere.obj")));

    Matrices lightMat;

    do {
        light1.GetTransform().SetPosition(uiData[2][0]);
        light1.SetPower(lightPower[0]);
        light1.SetRadius(lightRadius[0]);

        light2.GetTransform().SetPosition(uiData[3][0]);
        light2.SetPower(lightPower[1]);
        light2.SetRadius(lightRadius[1]);
        lightInfo[0] = light1.GetLightData();
        lightInfo[1] = light2.GetLightData();
        glm::vec2 delta = window.GetCursorDelta();
        window.UpdateCursorPosition();

        // shadow

        // make sure render size is same as texture
        glViewport(0, 0, SHADOW_SIZE, SHADOW_SIZE);
        Renderer::EnableDepthTest();
        Renderer::DisableCullFace();

        // every light
        for (int i = 0; i < lightDepths.size(); i++) {
            shadowFbo.AttachTexture(lightDepths[i]->GetTextureID(),
                                    GL_DEPTH_ATTACHMENT);
            shadowFbo.Bind();
            programShadow.Bind();
            Renderer::Clear();

            // not render light ball
            for (unsigned int j = 0; j < scene.size() - 2; j++) {
                scene[j].GetTransform().SetPosition(uiData[j][0]);
                scene[j].GetTransform().SetRotation(uiData[j][1]);
                scene[j].GetTransform().SetScale(uiData[j][2]);

                lightMat.model = scene[j].GetTransform().GetTransform();
                matrices.SetData(0, sizeof(lightMat), &lightMat);
                // use first one to render shadow
                lights.SetData(0, sizeof(LightData), lightInfo + i);
                programShadow.Validate();

                scene[j].Draw();
            }

            shadowFbo.Unbind();
        }

        // color (phong shader)
        glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

        camera.GetTransform().SetPosition(
            camera.GetTransform().GetPosition() +
            10 * window.GetScrollOffset().y *
                glm::normalize(camera.GetTransform().GetPosition()));

        if (window.GetMouseButton(GLFW_MOUSE_BUTTON_RIGHT)) {
            camera.RotateByDelta(delta.x * -2 / window.GetWidth(),
                                 delta.y * -2 / window.GetHeight());
        }

        glm::vec3 cameraPos = camera.GetTransform().GetPosition();
        camera.UpdateView();

        texMainColor.Bind(ALBEDO);
        reflectMap.Bind(REFLECT);
        // wallNormalMap.Bind(NORMAL);

        //deferred
        deferredFbo.Bind();
        programDeferredPass.Bind();
        // deferredFbo.AttachTexture(screenAlbedo.GetTextureID(), attachments[0]);
        // deferredFbo.AttachTexture(screenNormal.GetTextureID(),  attachments[1]);
        Renderer::Clear();
        glUniform3fv(cameraUniformDeferredPass, 1, &cameraPos.x);
        for (unsigned int i = 0; i < scene.size(); i++) {
            scene[i].GetTransform().SetPosition(uiData[i][0]);
            scene[i].GetTransform().SetRotation(uiData[i][1]);
            scene[i].GetTransform().SetScale(uiData[i][2]);

            Matrices mat1;

            mat1.model = scene[i].GetTransform().GetTransform();
            mat1.viewProjection = camera.GetViewProjection();
            matrices.SetData(0, sizeof(mat1), &mat1);
            materials.SetData(0, sizeof(Material), &matColor1);
            lights.SetData(0, sizeof(LightData) * LIGHT_NUMBER, &lightInfo);
            programDeferredPass.Validate();
            scene[i].Draw();
        }
        // deferredFbo.Unbind();
        programDeferredPass.Unbind();

        //deferred lighting 
        // deferredFbo.Bind();
        deferredLightFbo.Bind();
        programDeferredLight.Bind();
        screenAlbedo.Bind(ALBEDO);
        screenNormal.Bind(NORMAL);
        screenPosition.Bind(POSITION);
        screenDepth.Bind(DEPTH);
        for (int i = 0; i < lightDepths.size(); i++) {
            lightDepths[i]->Bind(SHADOW + i);
        }
        //geo
        planeVAO.Bind();
        Matrices mat2;
        mat2.model = scene[0].transform.GetTransform();
        mat2.viewProjection = camera.GetViewProjection();
        // LOG_INFO("mats {}",matrices.GetId());
        // LOG_INFO("material {}",materials.GetId());
        // LOG_INFO("light {}",lights.GetId());
        // matrices.SetData(0, sizeof(mat2), &mat2);
        materials.SetData(0, sizeof(Material), &matColor1);
        lights.SetData(0, sizeof(LightData) * LIGHT_NUMBER, &lightInfo);
        CameraData camData=camera.GetCameraData();
        cameraUbo.SetData(0, sizeof(CameraData), &camData);
        glUniform3fv(cameraUniformDeferredLight, 1, &cameraPos.x);


        // deferredFbo.AttachTexture(screenLight.GetTextureID(), attachments[0]);
        // deferredFbo.AttachTexture(screenVolume.GetTextureID(),  attachments[1]);
        Renderer::DisableDepthTest(); // direct render texture no need depth

        // LOG_DEBUG("camera {}",cameraUniformDeferredPass);
        programDeferredLight.Validate();
        Renderer::Clear();
        Renderer::Draw(planeVAO.GetIndexBuffer()->GetCount());
        programDeferredLight.Unbind();
        deferredLightFbo.Unbind();
        // deferredFbo.Unbind();

        // old lighting
        // colorFbo.Bind();
        // programColor.Bind();
        // Renderer::Clear();
        // Renderer::EnableDepthTest();
        // Renderer::DisableCullFace();

        // glUniform3fv(cameraUniform, 1, &cameraPos.x);
        // for (int i = 0; i < lightDepths.size(); i++) {
        //     lightDepths[i]->Bind(SHADOW + i);
        // }

        // for (unsigned int i = 0; i < scene.size(); i++) {
        //     scene[i].VAO->Bind();

        //     scene[i].transform.SetPosition(uiData[i][0]);
        //     scene[i].transform.SetRotation(uiData[i][1]);
        //     scene[i].transform.SetScale(uiData[i][2]);

        //     Matrices mat1;
        //     mat1.model = scene[i].transform.GetTransform();
        //     mat1.viewProjection = camera.GetViewProjection();
        //     matrices.SetData(0, sizeof(mat1), &mat1);
        //     materials.SetData(0, sizeof(Material), &matColor1);
        //     lights.SetData(0, sizeof(LightData) * LIGHT_NUMBER, &lightInfo);
        //     programColor.Validate();
        //     Renderer::Draw(scene[i].VAO->GetIndexBuffer()->GetCount());
        // }

        // // frame buffer part
        // programColor.Unbind();
        // colorFbo.Unbind();


        //screen space
        Renderer::DisableDepthTest(); // direct render texture no need depth
        programScreen.Bind();
        // renderSurface.Bind(0);
        screenLight.Bind(0);
        screenVolume.Bind(2);
        // screenAlbedo.Bind(0);
        // screenNormal.Bind(0);
        // screenAlbedo.Bind(2);
        // screenNormal.Bind(2);
        // screenVolume.Bind(2);
        // screenDepth.Bind(2);
        // screenPosition.Bind(2);
        // depthTexture.Bind(2);
        lightDepths[0]->Bind(1);

        programScreen.Validate();
        plane.Draw();
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

        ImGui::Begin("Light 1");
        ImGui::SliderFloat3("Position", &uiData[2][0][0], -300, 300);
        ImGui::SliderFloat3("Rotation", &uiData[2][1][0], 0, 360);
        ImGui::SliderFloat3("Scale", &uiData[2][2][0], 0.1f, 100.0f);
        ImGui::SliderFloat("Power", &lightPower[0], 0.1f, 10.0f);
        ImGui::SliderFloat("Radius", &lightRadius[0], 1.0f, 1000.0f);
        ImGui::End();

        ImGui::Begin("Light 2");
        ImGui::SliderFloat3("Position", &uiData[3][0][0], -300, 300);
        ImGui::SliderFloat3("Rotation", &uiData[3][1][0], 0, 360);
        ImGui::SliderFloat3("Scale", &uiData[3][2][0], 0.1f, 100.0f);
        ImGui::SliderFloat("Power", &lightPower[1], 0.1f, 10.0f);
        ImGui::SliderFloat("Radius", &lightRadius[1], 1.0f, 1000.0f);
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
