#ifndef WINDOW_HPP
#define WINDOW_HPP

#include "pch.hpp"

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

/**
 * An OOP wrapper for `GLFWwindow`
 * using OpenGL 4.6 core profile
 */
class Window {
public:
    Window(int width, int height, const char *title = "Indigo");
    ~Window();

    /**
     * Band-aid function to make the program
     * work before everything is structured
     */
    GLFWwindow *GetWindow() const { return m_Window; }

    void UpdateDimensions();
    void UpdateCursorPosition();

    float GetAspectRatio() const { return (float)m_Width / m_Height; }
    int GetWidth() const { return m_Width; }
    int GetHeight() const { return m_Height; }

    bool GetKey(int key) const;
    bool GetMouseButton(int button) const;
    glm::vec2 GetCursorDelta() const;
    glm::vec2 GetScrollOffset() const { return m_ScrollOffset; }

    bool ShouldClose() const;

    void PollEvents();

    friend void GLFWScrollCallback(GLFWwindow *window, double offsetX,
                                   double offsetY);

private:
    GLFWwindow *m_Window;

    int m_Width;
    int m_Height;

    double m_XPos;
    double m_YPos;

    glm::vec2 m_ScrollOffset;
};

#endif