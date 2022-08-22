#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

/**
 * An OOP wrapper for `GLFWwindow`
 * using OpenGL 4.6 core profile
 */
class Window {
public:
    Window(int width = 1024, int height = 768, const char *title = "Indigo");
    ~Window();

    /**
     * Band-aid function to make the program
     * work before everything is structured
     */
    GLFWwindow *GetWindow() { return m_Window; }
    bool GetKey(int key) { return glfwGetKey(m_Window, key) != GLFW_PRESS; }

private:
    GLFWwindow *m_Window;
};

#endif