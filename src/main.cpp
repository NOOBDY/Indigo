#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "log.hpp"
#include "window.hpp"
#include "renderer.hpp"
#include "program.hpp"
#include "camera.hpp"
#include "vertex_buffer.hpp"
#include "index_buffer.hpp"
#include "uniform_buffer.hpp"

int main(int, char **) {
    Log::Init();

    Window window;

    Renderer::Init();
    Renderer::ClearColor(0.102f, 0.02f, 0.478f, 1.0f);

    GLuint vertexArrayID;
    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

    Program program("../assets/shaders/base.vert",
                    "../assets/shaders/base.frag");

    UniformBuffer matrices(sizeof(glm::mat4), 0);
    UniformBuffer data(sizeof(glm::vec3), 1);

    Camera camera(45.0f, window.GetAspectRatio());

    glm::mat4 model1 = glm::mat4(1.0f);
    glm::vec3 color1(0.8f, 0.5f, 0.0f);
    model1 = glm::translate(model1, glm::vec3(2, 0, 0));

    glm::mat4 model2 = glm::mat4(1.0f);
    glm::vec3 color2(0.0f, 0.8f, 0.8f);
    model2 = glm::translate(model2, glm::vec3(-2, 0, 0));

    // clang-format off
    std::vector<float> square = {
        -1.0f, -1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,
    };

    std::vector<int> index = {
        0, 1, 2,
        1, 2, 3,
    };
    // clang-format on

    VertexBuffer vertexBuffer(&square[0], square.size() * sizeof(float));
    IndexBuffer indexBuffer(&index[0], index.size() * sizeof(int));

    do {
        Renderer::Clear();

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

        model1 = glm::rotate(model1, glm::radians(-1.0f), glm::vec3(0, 1, 0));
        glm::mat4 MVP1 = camera.GetViewProjection() * model1;

        matrices.SetData(0, sizeof(glm::mat4), &MVP1[0][0]);
        data.SetData(0, sizeof(glm::vec3), &color1[0]);

        glDrawElements(GL_TRIANGLES, indexBuffer.GetCount(), GL_UNSIGNED_INT,
                       (void *)0);

        model2 = glm::rotate(model2, glm::radians(-3.0f), glm::vec3(0, 0, 1));
        glm::mat4 MVP2 = camera.GetViewProjection() * model2;

        matrices.SetData(0, sizeof(glm::mat4), &MVP2[0][0]);
        data.SetData(0, sizeof(glm::vec3), &color2[0]);

        glDrawElements(GL_TRIANGLES, indexBuffer.GetCount(), GL_UNSIGNED_INT,
                       (void *)0);

        glDisableVertexAttribArray(0);

        glfwSwapBuffers(window.GetWindow());
        glfwPollEvents();
    } while (!window.ShouldClose());

    glDeleteVertexArrays(1, &vertexArrayID);
}
