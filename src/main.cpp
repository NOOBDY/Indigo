#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <spdlog/spdlog.h>

int main(int, char **) {
    glewExperimental = true;
    if (!glfwInit()) {
        std::cerr << "Failed to Initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(1024, 768, "Placeholder", NULL, NULL);

    if (!window) {
        std::cerr << "Failed to Create Window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    spdlog::info("OpenGL Info:");
    spdlog::info("  Vendor: {}", glGetString(GL_VENDOR));
    spdlog::info("  Renderer: {}", glGetString(GL_RENDERER));
    spdlog::info("  Version: {}", glGetString(GL_VERSION));

    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to Initialize GLEW\n";
        glfwTerminate();
        return -1;
    }

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    glClearColor(0.102f, 0.02f, 0.478f, 1.0f);

    do {
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
             glfwWindowShouldClose(window) == 0);
}
