#include <iostream>
#include <string>
#include <filesystem>

using String = std::string;

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "log.hpp"

int main(int, char **) {
    Log::Init();

    Assimp::Importer importer;

    glewExperimental = true;
    if (!glfwInit()) {
        LOG_ERROR("Failed to Initialize GLFW");
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(1024, 768, "Indigo main", NULL, NULL);

    if (!window) {
        LOG_ERROR("Failed to Create Window");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        LOG_ERROR("Failed to Initialize GLEW\n");
        glfwTerminate();
        return -1;
    }

    LOG_INFO("Vendor: {}", glGetString(GL_VENDOR));
    LOG_INFO("Renderer: {}", glGetString(GL_RENDERER));
    LOG_INFO("Version: {}", glGetString(GL_VERSION));

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

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

        glfwSwapBuffers(window);
        glfwPollEvents();
    } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
             glfwWindowShouldClose(window) == 0);
}
