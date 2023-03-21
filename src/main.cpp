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

    std::shared_ptr<Light> light1Test =
        std::make_shared<Light>(Light::POINT, glm::vec3(1.0f));
    std::shared_ptr<Light> light2Test =
        std::make_shared<Light>(Light::POINT, glm::vec3(1.0f));

    Scene scene(mainCamera);

    scene.AddLight(light1Test);
    scene.AddLight(light2Test);

    // begin model 1

    std::vector<Controller::TransformSlider> transformSliders;
    std::vector<Controller::LightSlider> lightSliders;

    transformSliders.push_back(Controller::TransformSlider("Model 1",       //
                                                           {0, 0, 0},       //
                                                           {180, 180, 180}, //
                                                           {1, 1, 1}));

    std::shared_ptr<Model> main;

    try {
        main = std::make_shared<Model>(
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
    transformSliders.push_back(Controller::TransformSlider("Model 2",       //
                                                           {0, 0, 0},       //
                                                           {180, 180, 180}, //
                                                           {1, 1, 1}));

    std::shared_ptr<Model> interior;

    try {
        interior = std::make_shared<Model>(
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

    // light 1
    transformSliders.push_back(                     //
        Controller::TransformSlider("Light 1",      //
                                    {50, 100, 200}, //
                                    {0, 0, 0},      //
                                    {20, 20, 20}));
    lightSliders.push_back(Controller::LightSlider("Light 1", 1, 500));

    // light 2
    transformSliders.push_back(                     //
        Controller::TransformSlider("Light 2",      //
                                    {-300, 300, 0}, //
                                    {0, 0, 0},      //
                                    {20, 20, 20}));
    lightSliders.push_back(Controller::LightSlider("Light 2", 2, 500));

    std::shared_ptr<Model> light1Sphere;
    std::shared_ptr<Model> light2Sphere;

    try {
        light1Sphere = std::make_shared<Model>(
            Importer::LoadFile("../assets/models/sphere.obj"));

        light1Sphere->SetCastShadows(false);

        scene.AddModel(light1Sphere);
    } catch (std::exception &e) {
        LOG_ERROR("{}", e.what());
    }

    try {
        light2Sphere = std::make_shared<Model>(
            Importer::LoadFile("../assets/models/sphere.obj"));

        light2Sphere->SetCastShadows(false);

        scene.AddModel(light2Sphere);
    } catch (std::exception &e) {
        LOG_ERROR("{}", e.what());
    }
    const auto templ = scene.GetModels();
    for (unsigned i = 0; i < scene.GetModels().size(); i++) {
        const auto model = scene.GetModels()[i];
        model->SetAlbedoTexture(texMainColor);
        model->SetUseAlbedoTexture(true);
    }

    do {
        glm::vec2 delta = window.GetCursorDelta();
        window.UpdateCursorPosition();

        const auto activeCamera = scene.GetActiveCamera();
        const auto models = scene.GetModels();
        const auto lights = scene.GetLights();

        for (unsigned int i = 0; i < lights.size(); i++) {
            lights[i]->SetTransform(
                transformSliders[i + 2].GetTransform()); // two non-light models
            lights[i]->SetPower(lightSliders[i].GetPower());
            lights[i]->SetRadius(lightSliders[i].GetRadius());
            // lightInfo[i] = lights[i]->GetLightData();
        }

        for (unsigned int i = 0; i < models.size(); i++) {
            models[i]->SetTransform(transformSliders[i].GetTransform());
        }

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

        transformSliders[0].Update();
        transformSliders[1].Update();

        transformSliders[2].Update();
        lightSliders[0].Update();

        transformSliders[3].Update();
        lightSliders[1].Update();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#pragma endregion

        // TODO: Figure this out and put it in `Window` class
        // glfwSwapInterval(0);
        glfwSwapBuffers(window.GetWindow());
        window.PollEvents();
    } while (!window.ShouldClose());

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
