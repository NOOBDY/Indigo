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

Window::Window(int width, int height, const char *title)
    : m_Width(width), m_Height(height) {
    LOG_TRACE("Creating Window");

    glfwSetErrorCallback(GLFWErrorCallback);

    if (glfwInit() != GLFW_TRUE) {
        LOG_ERROR("Failed to Initialize GLFW");
        throw;
    }

    glfwWindowHint(GLFW_SAMPLES, 4); // uses 4x MSAA
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // force disables tiling on
                                                // tiling WMs such as i3 or sway

    m_Window = glfwCreateWindow(m_Width, m_Height, title, NULL, NULL);

    if (!m_Window) {
        LOG_ERROR("Failed to Create Window");
        throw;
    }

    glfwMakeContextCurrent(m_Window);
    glfwSwapInterval(1); // enable v-sync

    glfwSetInputMode(m_Window, GLFW_STICKY_KEYS, GL_TRUE);
}

Window::~Window() {
    LOG_TRACE("Deleting Window");
    m_Window = NULL;
    glfwTerminate();
}

void Window::UpdateDimensions() {
    glfwGetWindowSize(m_Window, &m_Width, &m_Height);
}

void Window::UpdateCursorPosition() {
    glfwGetCursorPos(m_Window, &m_XPos, &m_YPos);
}

bool Window::GetKey(int key) const {
    return glfwGetKey(m_Window, key) == GLFW_PRESS;
}

bool Window::GetMouseButton(int button) const {
    return glfwGetMouseButton(m_Window, button);
}

glm::vec2 Window::GetCursorDelta() const {
    double xPos, yPos;

    glfwGetCursorPos(m_Window, &xPos, &yPos);

    return glm::vec2(xPos - m_XPos, yPos - m_YPos);
}

bool Window::ShouldClose() const {
    return GetKey(GLFW_KEY_ESCAPE) || GetKey(GLFW_KEY_Q) ||
           glfwWindowShouldClose(m_Window) == GLFW_TRUE;
}