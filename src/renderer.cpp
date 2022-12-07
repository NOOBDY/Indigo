#include "renderer.hpp"

#include "log.hpp"

// Force Windows devices with discrete GPU to enable it
#ifdef _WIN32
extern "C" {
__declspec(dllexport) int NvOptimusEnablement = 1;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

/**
 * Debug message handling callback function for OpenGL
 *
 * This will be called when there is an error for OpenGL
 */
void OpenGLDebugMessageCallback(GLenum source, GLenum type, GLuint id,
                                GLenum severity, GLsizei length,
                                const GLchar *message, const void *userParam) {
    std::string sourceString;
    std::string typeString;
    std::string severityString;

    // clang-format off
    switch (source) {
    case GL_DEBUG_SOURCE_API:               sourceString = "API"; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:     sourceString = "WINDOW SYSTEM"; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:   sourceString = "SHADER COMPILER"; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:       sourceString = "THIRD PARTY"; break;
    case GL_DEBUG_SOURCE_APPLICATION:       sourceString = "APPLICATION"; break;
    case GL_DEBUG_SOURCE_OTHER:             sourceString = "UNKNOWN"; break;
    default:                                sourceString = "UNKNOWN"; break;
    }

    switch (type) {
    case GL_DEBUG_TYPE_ERROR:               typeString = "ERROR"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: typeString = "DEPRECATED BEHAVIOR"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  typeString = "UNDEFINED BEHAVIOR"; break;
    case GL_DEBUG_TYPE_PORTABILITY:         typeString = "PORTABILITY"; break;
    case GL_DEBUG_TYPE_PERFORMANCE:         typeString = "PERFORMANCE"; break;
    case GL_DEBUG_TYPE_OTHER:               typeString = "OTHER"; break;
    case GL_DEBUG_TYPE_MARKER:              typeString = "MARKER"; break;
    default:                                typeString = "UNKNOWN"; break;
    }

    switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:            severityString = "HIGH"; break;
    case GL_DEBUG_SEVERITY_MEDIUM:          severityString = "MEDIUM"; break;
    case GL_DEBUG_SEVERITY_LOW:             severityString = "LOW"; break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:    severityString = "NOTIFICATION"; break;
    default:                                severityString = "UNKNOWN"; break;
    }
    // clang-format on

    switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
        LOG_ERROR("OpenGL severity {}", severityString);
        LOG_ERROR(" source: {}", sourceString);
        LOG_ERROR(" type: {}", typeString);
        LOG_ERROR(" message: {}", message);
        break;

    case GL_DEBUG_SEVERITY_MEDIUM:
        LOG_WARN("OpenGL severity {}", severityString);
        LOG_WARN(" source: {} type: {}", sourceString);
        LOG_WARN(" type: {}", typeString);
        LOG_WARN(" message: {}", message);
        break;

    case GL_DEBUG_SEVERITY_LOW:
        LOG_INFO("OpenGL severity {}", severityString);
        LOG_INFO(" source: {}", sourceString);
        LOG_INFO(" type: {}", typeString);
        LOG_INFO(" message: {}", message);
        break;

    case GL_DEBUG_SEVERITY_NOTIFICATION:
        LOG_DEBUG("OpenGL severity {}", severityString);
        LOG_DEBUG(" source: {}", sourceString);
        LOG_DEBUG(" type: {}", typeString);
        LOG_DEBUG(" message: {}", message);
        break;

    default:
        LOG_DEBUG("OpenGL severity {}", severityString);
        LOG_DEBUG(" source: {}", sourceString);
        LOG_DEBUG(" type: {}", typeString);
        LOG_DEBUG(" message: {}", message);
        break;
    }
}

void Renderer::Init() {
    LOG_TRACE("Initializing OpenGL");

    if (glewInit() != GLEW_OK)
        LOG_ERROR("Failed to Initialize GLEW\n");

#ifndef GL_ARB_direct_state_access
    LOG_ERROR("OpenGL driver doesn't support ARB_direct_state_access");
#endif

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