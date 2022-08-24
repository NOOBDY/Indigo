#include "window.hpp"

#include "log.hpp"

/**
 * Error handling callback function for GLFW
 *
 * This will be called when there is an error for GLFW
 */
void GLFWErrorCallback(int, const char *err_str) {
    LOG_ERROR("{}", err_str);
}

Window::Window(int width, int height, const char *title) : m_Window(NULL) {
    LOG_TRACE("Creating Window");

    glfwSetErrorCallback(GLFWErrorCallback);

    if (glfwInit() != GLFW_TRUE) {
        LOG_ERROR("Failed to Initialize GLFW");
        throw;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_Window = glfwCreateWindow(width, height, title, NULL, NULL);

    if (!m_Window) {
        LOG_ERROR("Failed to Create Window");
        throw;
    }

    glfwMakeContextCurrent(m_Window);

    glfwSetInputMode(m_Window, GLFW_STICKY_KEYS, GL_TRUE);
}

Window::~Window() {
    LOG_TRACE("Deleting Window");
    m_Window = NULL;
    glfwTerminate();
}