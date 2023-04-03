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

    std::shared_ptr<Light> light1 =
        std::make_shared<Light>(Light::POINT,
                                Transform({50, 100, 200}, //
                                          {0, 0, 0},      //
                                          {20, 20, 20}),
                                1, 1000, glm::vec3(1.0f));
    std::shared_ptr<Light> light2 =
        std::make_shared<Light>(Light::POINT,
                                Transform({-300, 300, 0}, //
                                          {0, 0, 0},      //
                                          {20, 20, 20}),
                                2, 1000, glm::vec3(1.0f));

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

    std::shared_ptr<Model> light1Sphere;
    std::shared_ptr<Model> light2Sphere;

    try {
        light1Sphere = std::make_shared<Model>(
            "Light 1",                                         //
            Importer::LoadFile("../assets/models/sphere.obj"), //
            Transform({50, 100, 200},                          //
                      {0, 0, 0},                               //
                      {20, 20, 20}));

        light1Sphere->SetCastShadows(false);

        light1->SetPower(1);
        light1->SetRadius(1000);
        light1->SetTransform(light1Sphere->GetTransform());

        scene.AddModel(light1Sphere);
    } catch (std::exception &e) {
        LOG_ERROR("{}", e.what());
    }

    try {
        light2Sphere = std::make_shared<Model>(
            "Light 2",                                         //
            Importer::LoadFile("../assets/models/sphere.obj"), //
            Transform({-300, 300, 0},                          //
                      {0, 0, 0},                               //
                      {20, 20, 20}));

        light2Sphere->SetCastShadows(false);

        light2->SetPower(2);
        light2->SetRadius(1000);
        light2->SetTransform(light2Sphere->GetTransform());

        scene.AddModel(light2Sphere);
    } catch (std::exception &e) {
        LOG_ERROR("{}", e.what());
    }

    for (unsigned i = 0; i < scene.GetModels().size(); i++) {
        const auto model = scene.GetModels()[i];
        model->SetAlbedoTexture(texMainColor);
        model->SetUseAlbedoTexture(true);
        // model->SetNormalTexture(wallNormalMap);
        // model->SetUseNormalTexture(true);
        // model->SetVisible(false);
        // model->SetCastShadows(false);
    }

    scene.AddLight(light1);
    scene.AddLight(light2);

    for (const auto &obj : scene.GetModels()) {
        LOG_DEBUG("{}", obj->GetLabel());
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
        const std::vector<std::shared_ptr<Model>> models = scene.GetModels();
        const std::vector<std::shared_ptr<Light>> lights = scene.GetLights();

        // texMainColor->Bind(Pipeline::ALBEDO);
        pipeline.Render(scene);

        activeCamera->GetTransform().SetPosition(
            activeCamera->GetTransform().GetPosition() +
            10 * window.GetScrollOffset().y *
                glm::normalize(activeCamera->GetTransform().GetPosition()));

        if (window.GetMouseButton(GLFW_MOUSE_BUTTON_RIGHT)) {
            activeCamera->RotateByDelta(delta.x * -2 / window.GetWidth(),
                                        delta.y * -2 / window.GetHeight());
        }

        activeCamera->UpdateView();
#pragma region GUI
        Renderer::DisableDepthTest();
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

        auto object = scene.GetActiveSceneObject();

        if (object) {
            glm::vec3 position = object->GetTransform().GetPosition();
            glm::vec3 rotation = object->GetTransform().GetRotation();
            glm::vec3 scale = object->GetTransform().GetScale();
            ImGui::Begin("Transform");
            ImGui::SetWindowSize({270, 100});
            ImGui::SetWindowPos({10, 10});
            ImGui::SliderFloat3("Position", &position[0], -300, 300);
            ImGui::SliderFloat3("Rotation", &rotation[0], 0, 360);
            ImGui::SliderFloat3("Scale", &scale[0], 0.1f, 5.0f);
            ImGui::End();

            if (object->GetObjectType() == SceneObject::LIGHT) {
                auto light = std::dynamic_pointer_cast<Light>(object);
                auto power = light->GetPower();
                auto radius = light->GetRadius();

                ImGui::Begin("Light Attributes");
                ImGui::SetWindowSize({270, 85});
                ImGui::SetWindowPos({10, 115});
                ImGui::SliderFloat("Power", &power, 0.1f, 10.0f);
                ImGui::SliderFloat("Radius", &radius, 1.0f, 1000.0f);
                ImGui::End();

                light->SetPower(power);
                light->SetRadius(radius);
            }

            object->SetTransform(Transform(position, rotation, scale));
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
