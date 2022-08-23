#include <iostream>
#include <string>

using String = std::string;

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "log.hpp"
#include "window.hpp"

int main(int, char **) {
    Log::Init();
    Log::SetLevel(Log::DEBUG);

    Assimp::Importer importer;

    glewExperimental = true;

    Window window(1024, 768);

    if (glewInit() != GLEW_OK) {
        LOG_ERROR("Failed to Initialize GLEW\n");
        glfwTerminate();
        return -1;
    }

    LOG_INFO("Vendor: {}", glGetString(GL_VENDOR));
    LOG_INFO("Renderer: {}", glGetString(GL_RENDERER));
    LOG_INFO("Version: {}", glGetString(GL_VERSION));

    glClearColor(0.102f, 0.02f, 0.478f, 1.0f);

    const String filepath = "../assets/sphere.obj";

    const aiScene *scene =
        importer.ReadFile(filepath, aiProcessPreset_TargetRealtime_Fast);

    if (!scene) {
        LOG_ERROR("Failed Loading File: '{}'", filepath);
        glfwTerminate();
        return -1;
    }

    LOG_INFO("Loading File: '{}'", filepath);

    if (scene->HasMeshes()) {
        aiMesh *mesh = scene->mMeshes[0];

        LOG_INFO("Loading {} Vertices", mesh->mNumVertices);
    }

    do {
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window.GetWindow());
        glfwPollEvents();
    } while (window.GetKey(GLFW_KEY_ESCAPE) &&
             glfwWindowShouldClose(window.GetWindow()) == 0);
}
