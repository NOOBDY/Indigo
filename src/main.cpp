#include "pch.hpp"

#include <map>

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
    Renderer::ClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    Controller::InitGUI(window);

    Pipeline pipeline(SCREEN_WIDTH, SCREEN_HEIGHT);

    pipeline.Init();

    std::shared_ptr<Texture> texMainColor = std::make_shared<Texture>(
        "../assets/textures/little_city/main_color.jpg");
    std::shared_ptr<Texture> texInterior = std::make_shared<Texture>(
        "../assets/textures/little_city/interior.jpg");
    std::shared_ptr<Texture> texMisc =
        std::make_shared<Texture>("../assets/textures/little_city/misc.png");
    std::shared_ptr<Texture> reflectMap =
        std::make_shared<Texture>("../assets/textures/vestibule_2k.hdr");
    std::shared_ptr<Texture> wallNormalMap = std::make_shared<Texture>(
        "../assets/textures/T_Wall_Damaged_2x1_A_N.png");
    std::shared_ptr<Texture> wallAOMap = std::make_shared<Texture>(
        "../assets/textures/T_Wall_Damaged_2x1_A_AO.png");

    std::shared_ptr<Camera> mainCamera =
        std::make_shared<Camera>(glm::vec3{350, 200, 100}, 45.0f,
                                 window.GetAspectRatio(), 10.0f, 5000.0f);
    // mainCamera->GetTransform().SetPosition({270, 200, 100});
    Scene scene(mainCamera);

    try {
        auto models =
            Importer::LoadFileScene("../assets/models/sponza/Sponza.gltf");
        for (auto &i : models) {
            i->SetTransform(Transform({0, 0, 0}, {0, 0, 0}, {.3, .3, .3}));
            scene.AddModel(i);
        }
    } catch (std::exception &e) {
        LOG_ERROR("{}", e.what());
    }

    try {
        std::shared_ptr<Light> light1 = std::make_shared<Light>( //
            "Point Light",                                       //
            Light::POINT,                                        //
            Transform({5, 10, 20},                               //
                      {0, 0, 0},                                 //
                      {5, 5, 5}),
            1, 1000, glm::vec3(1.0f));
        // bigger texture size for direction shadow
        light1->SetShadowSize(512);

        scene.AddLight(light1);
    } catch (std::exception &e) {
        LOG_ERROR("{}", e.what());
    }

    try {
        std::shared_ptr<Light> light2 = std::make_shared<Light>( //
            "Direction Light",                                   //
            Light::DIRECTION,                                    //
            Transform({0, 500, 0},                               //
                      {30, 180, 0},                              //
                      {5, 5, 5}),
            2, 1000, glm::vec3(1.0f));

        light2->SetShadowSize(1024);
        light2->SetColorTexture(reflectMap);

        scene.AddLight(light2);
    } catch (std::exception &e) {
        LOG_ERROR("{}", e.what());
    }
    try {
        std::shared_ptr<Light> light3 = std::make_shared<Light>( //
            "Ambient Light",                                     //
            Light::AMBIENT,                                      //
            Transform({0.0, 30, 0},                              //
                      {0, 0, 0},                                 //
                      {5, 5, 5}),
            1, 1000, glm::vec3(1.0f), false);

        // light3->SetShadowSize(2048);
        light3->SetColorTexture(reflectMap);
        light3->SetUseColorTexture(true);

        scene.AddLight(light3);
    } catch (std::exception &e) {
        LOG_ERROR("{}", e.what());
    }

    scene.SetEnvironmentMap(reflectMap);

    do {
        static bool invertX = false;
        static bool invertY = false;

        const auto &io = ImGui::GetIO();
        const glm::vec2 delta = window.GetCursorDelta();
        window.UpdateCursorPosition();

        const std::shared_ptr<Camera> activeCamera = scene.GetActiveCamera();

        // `io.WantCaptureMouse` shows if the cursor is on any `ImGui` window
        if (!io.WantCaptureMouse) {
            activeCamera->Zoom(20 * window.GetScrollOffset().y);

            if (window.GetMouseButton(GLFW_MOUSE_BUTTON_LEFT)) {
                activeCamera->Rotate((invertX ? -1 : 1) * delta.x * 0.1f,
                                     (invertY ? -1 : 1) * delta.y * 0.1f);
            }

            if (window.GetMouseButton(GLFW_MOUSE_BUTTON_RIGHT)) {
                activeCamera->Pan(delta.x * -0.5f, delta.y * 0.5f);
            }

            if (window.GetMouseButton(GLFW_MOUSE_BUTTON_MIDDLE)) {
                const glm::vec2 pos =
                    glm::min(window.GetCursorPos(),
                             glm::vec2{window.GetWidth(), window.GetHeight()});

                const unsigned int id = pipeline.GetIdByPosition(pos);

                scene.SetActiveSceneObject(id);
            }
        }

        activeCamera->UpdateView();

        pipeline.Render(scene);

        Renderer::DisableDepthTest();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

#pragma region Debug Info UI
        ImGui::Begin("Debug Info");
        ImGui::SetWindowPos({SCREEN_WIDTH - 110, 10});
        ImGui::SetWindowSize({100, 85});
        ImGui::Text("%.1f FPS", io.Framerate);
        ImGui::Text("(%d, %d)", (int)delta.x, (int)delta.y);
        ImGui::Text("(%d, %d)", (int)window.GetScrollOffset().x,
                    (int)window.GetScrollOffset().y);
        ImGui::End();
#pragma endregion

#pragma region Objects UI
        ImGui::Begin("Objects");
        ImGui::SetWindowPos({SCREEN_WIDTH - 140, 100});
        ImGui::SetWindowSize({130, 200});
        for (const auto &object : scene.GetSceneObjects()) {
            auto id = scene.GetActiveSceneObjectID();
            if (ImGui::Selectable(object->GetLabel().c_str(),
                                  static_cast<int>(object->GetID()) == id)) {
                scene.SetActiveSceneObject(object->GetID());
            }
        }
        ImGui::End();
#pragma endregion

#pragma region Pipeline UI
        ImGui::Begin("Pipeline");
        ImGui::SetWindowPos({SCREEN_WIDTH - 140, 305});
        ImGui::SetWindowSize({130, 150});
        // ImGui::BeginCombo("Pass", "");
        const std::map<Pipeline::Pass, std::string> passes{
            {Pipeline::Pass::ALBEDO, "Albedo"},
            {Pipeline::Pass::EMISSION, "Emission"},
            {Pipeline::Pass::NORMAL, "Normal"},
            {Pipeline::Pass::ARM, "ARM"},
            {Pipeline::Pass::POSITION, "Position"},
            {Pipeline::Pass::ID, "ID"},
            {Pipeline::Pass::DEPTH, "Depth"},
            {Pipeline::Pass::SSAO, "SSAO"},
            {Pipeline::Pass::LIGHTING, "Lighting"},
            {Pipeline::Pass::VOLUME, "Volume"},
            {Pipeline::Pass::SCREEN, "Screen"},
        };

        Pipeline::Pass selectedPass = pipeline.GetActivePass();

        if (ImGui::BeginCombo("Pass", passes.at(selectedPass).c_str())) {
            for (const auto &pass : passes) {
                const bool isSelected = selectedPass == pass.first;

                if (ImGui::Selectable(pass.second.c_str(), isSelected))
                    selectedPass = pass.first;

                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            pipeline.SetActivePass(selectedPass);

            ImGui::EndCombo();
        }

        if (selectedPass == Pipeline::Pass::SCREEN) {
            bool useSSAO = pipeline.GetUseSSAO();
            bool useOutline = pipeline.GetUseOutline();
            bool useHDRI = pipeline.GetUseHDRI();

            ImGui::Checkbox("SSAO", &useSSAO);
            ImGui::Checkbox("Outline", &useOutline);
            ImGui::Checkbox("HDRI", &useHDRI);

            pipeline.SetUseSSAO(useSSAO);
            pipeline.SetUseOutline(useOutline);
            pipeline.SetUseHDRI(useHDRI);
        }
        ImGui::End();
#pragma endregion

#pragma region Camera UI
        ImGui::Begin("Camera");
        ImGui::SetWindowPos({SCREEN_WIDTH - 140, 460});
        ImGui::SetWindowSize({130, 80});

        ImGui::Checkbox("Invert X", &invertX);
        ImGui::Checkbox("Invert Y", &invertY);

        ImGui::End();
#pragma endregion

#pragma region Active Object UI
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
