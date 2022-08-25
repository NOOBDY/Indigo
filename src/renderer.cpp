#include "renderer.hpp"

#include <GL/glew.h>

#include "log.hpp"

void OpenGLErrorCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
                         GLsizei length, const GLchar *message,
                         const void *userParam) {
    switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
    case GL_DEBUG_SEVERITY_MEDIUM:
        LOG_ERROR("{}", message);
        throw;
    case GL_DEBUG_SEVERITY_LOW:
        LOG_WARN("{}", message);
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        LOG_INFO("{}", message);
    }
}

void Renderer::Init() {
    LOG_TRACE("Initializing OpenGL");

    if (glewInit() != GLEW_OK) {
        LOG_ERROR("Failed to Initialize GLEW\n");
        throw;
    }

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(OpenGLErrorCallback, 0);

    LOG_TRACE("Successfully initialized OpenGL");
    LOG_INFO("Vendor: {}", glGetString(GL_VENDOR));
    LOG_INFO("Renderer: {}", glGetString(GL_RENDERER));
    LOG_INFO("Version: {}", glGetString(GL_VERSION));
}

void Renderer::ClearColor(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
}

void Renderer::Clear() {
    glClear(GL_COLOR_BUFFER_BIT);
}