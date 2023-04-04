#include "pch.hpp"

#include "log.hpp"
#include "exception.hpp"
#include "window.hpp"
#include "renderer.hpp"
#include "pipeline.hpp"
#include "controller.hpp"
#include "camera.hpp"
#include "importer.hpp"
#include "vertex_array.hpp"
#include "texture.hpp"
#include "transform.hpp"
#include "scene.hpp"
#include "model.hpp"
#include "light.hpp"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

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
    Renderer::ClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    Controller::InitGUI(window);

    Pipeline pipeline(SCREEN_WIDTH, SCREEN_HEIGHT);

    pipeline.Init();

    std::shared_ptr<Camera> mainCamera =
        std::make_shared<Camera>(45.0f, window.GetAspectRatio());

    Scene scene(mainCamera);

    // begin model 1
    std::shared_ptr<Model> main;

    try {
        main = std::make_shared<Model>(
            "Model 1",
            Importer::LoadFile("../assets/models/little_city/main.glb"),
            Transform({0, 0, 0},       //
                      {180, 180, 180}, //
                      {1, 1, 1}));

        scene.AddModel(main);
    } catch (std::exception &e) {
        LOG_ERROR("{}", e.what());
    }
    // end model 1

    // begin model 2
    std::shared_ptr<Model> interior;

    try {
        interior = std::make_shared<Model>(
            "Model 2",
            Importer::LoadFile("../assets/models/little_city/interior.glb"),
            Transform({0, 0, 0},       //
                      {180, 180, 180}, //
                      {1, 1, 1}));

        scene.AddModel(interior);
    } catch (std::exception &e) {
        LOG_ERROR("{}", e.what());
    }
    // end model 2

    std::shared_ptr<Texture> texMainColor = std::make_shared<Texture>(
        "../assets/textures/little_city/main_color.jpg");
    std::shared_ptr<Texture> texInterior = std::make_shared<Texture>(
        "../assets/textures/little_city/interior.jpg");
    std::shared_ptr<Texture> reflectMap =
        std::make_shared<Texture>("../assets/textures/vestibule_2k.hdr");
    std::shared_ptr<Texture> wallNormalMap = std::make_shared<Texture>(
        "../assets/textures/T_Wall_Damaged_2x1_A_N.png");
    std::shared_ptr<Texture> wallAOMap = std::make_shared<Texture>(
        "../assets/textures/T_Wall_Damaged_2x1_A_AO.png");

    try {
        std::shared_ptr<Light> light1 = std::make_shared<Light>( //
            "Light 1",                                           //
            Light::DIRECTION,                                    //
            Transform({50, 100, 200},                            //
                      {0, 0, 0},                                 //
                      {20, 20, 20}),
            1, 1000, glm::vec3(1.0f));
        light1->SetShadowSize(2048);
        scene.AddLight(light1);
    } catch (std::exception &e) {
        LOG_ERROR("{}", e.what());
    }

    try {
        std::shared_ptr<Light> light2 = std::make_shared<Light>( //
            "Light 2",                                           //
            Light::POINT,                                        //
            Transform({-300, 300, 0},                            //
                      {0, 0, 0},                                 //
                      {20, 20, 20}),
            2, 1000, glm::vec3(1.0f));

        scene.AddLight(light2);
    } catch (std::exception &e) {
        LOG_ERROR("{}", e.what());
    }

    for (const auto &model : scene.GetModels()) {
        model->SetAlbedoTexture(texMainColor);
        // model->SetNormalTexture(wallNormalMap);
        model->SetUseAlbedoTexture(true);
        model->SetNormalTexture(wallNormalMap);
        // model->SetUseNormalTexture(true);
        // model->SetVisible(false);
        // model->SetCastShadows(false);
    }

    do {
        auto &io = ImGui::GetIO();
        glm::vec2 delta = window.GetCursorDelta();
        window.UpdateCursorPosition();

        // `io.WantCaptureMouse` shows if the cursor is on any `ImGui` window
        if (window.GetMouseButton(GLFW_MOUSE_BUTTON_LEFT) &&
            !io.WantCaptureMouse) {
            unsigned int id = pipeline.GetIdByPosition(window.GetCursorPos());
            scene.SetActiveSceneObject(id);
        }

        const std::shared_ptr<Camera> activeCamera = scene.GetActiveCamera();

        // texMainColor->Bind(Pipeline::ALBEDO);
        pipeline.Render(scene);

        if (!io.WantCaptureMouse) {
            activeCamera->GetTransform().SetPosition(
                activeCamera->GetTransform().GetPosition() +
                10 * window.GetScrollOffset().y *
                    glm::normalize(activeCamera->GetTransform().GetPosition()));

            if (window.GetMouseButton(GLFW_MOUSE_BUTTON_RIGHT)) {
                activeCamera->RotateByDelta(delta.x * -2 / window.GetWidth(),
                                            delta.y * -2 / window.GetHeight());
            }
        }

        activeCamera->UpdateView();
#pragma region GUI
        Renderer::DisableDepthTest();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        float framerate = ImGui::GetIO().Framerate;

        ImGui::Begin("Debug Info");
        ImGui::SetWindowPos({1170, 10});
        ImGui::SetWindowSize({100, 85});
        ImGui::Text("%.1f FPS", framerate);
        ImGui::Text("(%d, %d)", (int)delta.x, (int)delta.y);
        ImGui::Text("(%d, %d)", (int)window.GetScrollOffset().x,
                    (int)window.GetScrollOffset().y);
        ImGui::End();

        ImGui::Begin("Objects");
        ImGui::SetWindowPos({1140, 100});
        ImGui::SetWindowSize({130, 200});
        for (const auto &object : scene.GetSceneObjects()) {
            auto id = scene.GetActiveSceneObjectID();
            if (ImGui::Selectable(object->GetLabel().c_str(),
                                  object->GetID() == id)) {
                scene.SetActiveSceneObject(object->GetID());
            }
        }
        ImGui::End();

        auto activeObject = scene.GetActiveSceneObject();

        if (activeObject) {
            Controller::TransformGUI(activeObject);

            switch (activeObject->GetObjectType()) {
            case SceneObject::MODEL:
                Controller::ModelAttributeGUI(
                    std::dynamic_pointer_cast<Model>(activeObject));
                break;

            case SceneObject::LIGHT: {
                Controller::LightAttributeGUI(
                    std::dynamic_pointer_cast<Light>(activeObject));
                break;
            }
            }
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#pragma endregion

        // TODO: Figure this out and put it in `Window` class
        // glfwSwapInterval(0);
        glfwSwapBuffers(window.GetWindow());
        window.PollEvents();
    } while (!window.ShouldClose());
    // pipeline.SavePass(Pipeline::LIGHTING, "lighting.png");

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
