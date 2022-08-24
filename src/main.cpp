#include <iostream>

#include "log.hpp"
#include "window.hpp"
#include "program.hpp"

int main(int, char **) {
    Log::Init();
    Log::SetLevel(Log::DEBUG);

    Window window(1024, 768);

    if (glewInit() != GLEW_OK) {
        LOG_ERROR("Failed to Initialize GLEW\n");
        glfwTerminate();
        return -1;
    }

    glClearColor(0.102f, 0.02f, 0.478f, 1.0f);

    GLuint vertexArrayID;
    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

    LOG_INFO("Vendor: {}", glGetString(GL_VENDOR));
    LOG_INFO("Renderer: {}", glGetString(GL_RENDERER));
    LOG_INFO("Version: {}", glGetString(GL_VERSION));

    Program program("../assets/shaders/base.vert",
                    "../assets/shaders/base.frag");
    // clang-format off
    static const GLfloat triangle[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f,  -1.0f, 0.0f,
        0.0f,  1.0f,  0.0f,
    };
    // clang-format on

    GLuint vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);

    GLenum err;

    do {
        glClear(GL_COLOR_BUFFER_BIT);

        program.Bind();

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glVertexAttribPointer( //
            0,                 //
            3,                 //
            GL_FLOAT,          //
            GL_FALSE,          //
            0,                 //
            (void *)0          //
        );

        glDrawArrays(GL_TRIANGLES, 0, 3);
        glDisableVertexAttribArray(0);

        glfwSwapBuffers(window.GetWindow());
        glfwPollEvents();

        err = glGetError();

        if (err != GL_NO_ERROR) {
            LOG_ERROR("GL Error 0x{:x}: {}", err, gluErrorString(err));
            break;
        }
    } while (window.GetKey(GLFW_KEY_ESCAPE) &&
             glfwWindowShouldClose(window.GetWindow()) == 0);

    glDeleteBuffers(1, &vertexBuffer);
    glDeleteVertexArrays(1, &vertexArrayID);
}
