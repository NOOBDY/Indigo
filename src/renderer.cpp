#include "renderer.hpp"

#include "log.hpp"

/**
 * Debug message handling callback function for OpenGL
 *
 * This will be called when there is an error for OpenGL
 */
void OpenGLDebugMessageCallback(GLenum source, GLenum type, GLuint id,
                                GLenum severity, GLsizei length,
                                const GLchar *message, const void *userParam) {
    switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
        LOG_ERROR("{}", message);
        throw;
    case GL_DEBUG_SEVERITY_MEDIUM:
        LOG_WARN("{}", message);
        break;
    case GL_DEBUG_SEVERITY_LOW:
        LOG_INFO("{}", message);
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        LOG_DEBUG("{}", message);
        break;
    }
}

void Renderer::Init() {
    LOG_TRACE("Initializing OpenGL");

    if (glewInit() != GLEW_OK)
        LOG_ERROR("Failed to Initialize GLEW\n");

    if (!GL_ARB_direct_state_access)
        LOG_ERROR("OpenGL driver doesn't support ARB_direct_state_access");

    glEnable(GL_CULL_FACE);

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(OpenGLDebugMessageCallback, 0);

    LOG_TRACE("Successfully Initialized OpenGL");
    LOG_INFO("OpenGL Info");
    LOG_INFO("  Vendor: {}", glGetString(GL_VENDOR));
    LOG_INFO("  Renderer: {}", glGetString(GL_RENDERER));
    LOG_INFO("  Version: {}", glGetString(GL_VERSION));
}

void Renderer::ClearColor(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
}

void Renderer::Clear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::Draw(const unsigned int indexCount) {
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, (void *)0);
}

void Renderer::EnableDepthTest() {
    glEnable(GL_DEPTH_TEST);
}

void Renderer::DisableDepthTest() {
    glDisable(GL_DEPTH_TEST);
}

void Renderer::EnableCullFace() {
    glEnable(GL_CULL_FACE);
}

void Renderer::DisableCullFace() {
    glDisable(GL_CULL_FACE);
}