#include <vector>

#include "log.hpp"
#include "window.hpp"
#include "program.hpp"
#include "vertex_buffer.hpp"
#include "index_buffer.hpp"

int main(int, char **) {
    Log::Init();
    Log::SetLevel(Log::TRACE);

    Window window(1024, 768);

    if (glewInit() != GLEW_OK) {
        LOG_ERROR("Failed to Initialize GLEW\n");
        return -1;
    }

    glClearColor(0.102f, 0.02f, 0.478f, 1.0f);
    glEnable(GL_DEBUG_OUTPUT);

    GLuint vertexArrayID;
    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

    LOG_INFO("Vendor: {}", glGetString(GL_VENDOR));
    LOG_INFO("Renderer: {}", glGetString(GL_RENDERER));
    LOG_INFO("Version: {}", glGetString(GL_VERSION));

    Program program("../assets/shaders/base.vert",
                    "../assets/shaders/base.frag");
    // clang-format off
    std::vector<float> square = {
        -1.0f, -1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,
    };

    std::vector<uint32_t> index = {
        0, 1, 2,
        1, 2, 3,
    };
    // clang-format on

    VertexBuffer vertexBuffer(&square[0], square.size() * sizeof(float));
    IndexBuffer indexBuffer(&index[0], index.size() * sizeof(uint32_t));

    GLenum err;

    do {
        glClear(GL_COLOR_BUFFER_BIT);

        program.Bind();

        glEnableVertexAttribArray(0);
        vertexBuffer.Bind();
        glVertexAttribPointer( //
            0,                 //
            3,                 //
            GL_FLOAT,          //
            GL_FALSE,          //
            0,                 //
            (void *)0          //
        );

        indexBuffer.Bind();

        glDrawElements(GL_TRIANGLES, indexBuffer.GetCount(), GL_UNSIGNED_INT,
                       (void *)0);

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

    glDeleteVertexArrays(1, &vertexArrayID);
}
