#include <vector>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "log.hpp"
#include "window.hpp"
#include "renderer.hpp"
#include "program.hpp"
#include "camera.hpp"
#include "importer.hpp"
#include "vertex_array.hpp"
#include "vertex_buffer.hpp"
#include "index_buffer.hpp"
#include "uniform_buffer.hpp"
#include "texture.hpp"

int main(int, char **) {
    Log::Init();

    Window window;

    Renderer::Init();
    Renderer::ClearColor(0.102f, 0.02f, 0.478f, 1.0f);

    Program program("../assets/shaders/base.vert",
                    "../assets/shaders/base.frag");

    UniformBuffer matrices(sizeof(glm::mat4), 0);
    UniformBuffer data(sizeof(glm::vec3), 1);

    Camera camera(45.0f, window.GetAspectRatio());

    // begin model 1
    glm::mat4 model1 = glm::mat4(1.0f);
    glm::vec3 color1(0.8f, 0.5f, 0.0f);
    model1 = glm::translate(model1, glm::vec3(2, 0, 0));

    Importer obj1("../assets/donut.obj");

    std::shared_ptr<VertexBuffer> vertices1(
        std::make_shared<VertexBuffer>(obj1.GetVertices(), 3 * sizeof(float)));

    std::shared_ptr<VertexBuffer> uvs1(
        std::make_shared<VertexBuffer>(obj1.GetUVs(), 2 * sizeof(float)));

    std::shared_ptr<IndexBuffer> indices1(
        std::make_shared<IndexBuffer>(obj1.GetIndices()));
    // end model 1

    // begin model 2
    glm::mat4 model2 = glm::mat4(1.0f);
    glm::vec3 color2(0.0f, 0.8f, 0.8f);
    model2 = glm::translate(model2, glm::vec3(-2, 0, 0));

    Importer obj2("../assets/suzanne.obj");

    std::shared_ptr<VertexBuffer> vertices2(
        std::make_shared<VertexBuffer>(obj2.GetVertices(), 3 * sizeof(float)));

    std::shared_ptr<VertexBuffer> uvs2(
        std::make_shared<VertexBuffer>(obj2.GetUVs(), 2 * sizeof(float)));

    std::shared_ptr<IndexBuffer> indices2(
        std::make_shared<IndexBuffer>(obj2.GetIndices()));
    // end model 2

    Texture tex1("../assets/fabric.png");
    Texture tex2("../assets/uv.png");

    program.Bind();

    program.SetInt("texture1", 0);
    program.SetInt("texture2", 1);

    do {
        Renderer::Clear();

        //
        VertexArray vao1;
        vao1.AddVertexBuffer(vertices1);
        vao1.AddVertexBuffer(uvs1);
        vao1.SetIndexBuffer(indices1);

        model1 = glm::rotate(model1, glm::radians(-1.0f), glm::vec3(-1, 1, 0));
        glm::mat4 MVP1 = camera.GetViewProjection() * model1;

        matrices.SetData(0, sizeof(glm::mat4), &MVP1[0][0]);
        data.SetData(0, sizeof(glm::vec3), &color1[0]);

        tex1.Bind(0);
        tex2.Bind(1);

        glDrawElements(GL_TRIANGLES, vao1.GetIndexBuffer()->GetCount(),
                       GL_UNSIGNED_INT, (void *)0);
        //

        //
        VertexArray vao2;
        vao2.AddVertexBuffer(vertices2);
        vao2.AddVertexBuffer(uvs2);
        vao2.SetIndexBuffer(indices2);

        model2 = glm::rotate(model2, glm::radians(-1.0f), glm::vec3(0, 1, 0));
        glm::mat4 MVP2 = camera.GetViewProjection() * model2;

        matrices.SetData(0, sizeof(glm::mat4), &MVP2[0][0]);
        data.SetData(0, sizeof(glm::vec3), &color2[0]);

        tex2.Bind(0);
        tex1.Bind(1);

        glDrawElements(GL_TRIANGLES, vao2.GetIndexBuffer()->GetCount(),
                       GL_UNSIGNED_INT, (void *)0);

        glfwSwapBuffers(window.GetWindow());
        glfwPollEvents();
    } while (!window.ShouldClose());
}
