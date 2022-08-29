#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

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

    Assimp::Importer importer;

    glm::mat4 model1 = glm::mat4(1.0f);
    glm::vec3 color1(0.8f, 0.5f, 0.0f);
    model1 = glm::translate(model1, glm::vec3(2, 0, 0));

    const aiScene *scene1 =
        importer.ReadFile("../assets/donut.obj", aiProcess_Triangulate);

    LOG_DEBUG("{}", scene1->mMeshes[0]->mNumVertices);

    const aiMesh *mesh1 = scene1->mMeshes[0];

    VertexBuffer vbo1((float *)&mesh1->mVertices[0],
                      mesh1->mNumVertices * sizeof(aiVector3D));

    std::vector<int> indices1;

    for (int i = 0; i < mesh1->mNumFaces; ++i) {
        aiFace f = mesh1->mFaces[i];
        for (int j = 0; j < f.mNumIndices; ++j) {
            indices1.push_back(f.mIndices[j]);
        }
    }

    IndexBuffer ibo1(&indices1[0], indices1.size() * sizeof(int));
    //

    //
    glm::mat4 model2 = glm::mat4(1.0f);
    glm::vec3 color2(0.0f, 0.8f, 0.8f);
    model2 = glm::translate(model2, glm::vec3(-2, 0, 0));

    const aiScene *scene2 =
        importer.ReadFile("../assets/suzanne.obj", aiProcess_Triangulate);

    LOG_DEBUG("{}", scene2->mMeshes[0]->mNumVertices);

    const aiMesh *mesh2 = scene2->mMeshes[0];

    VertexBuffer vbo2((float *)&mesh2->mVertices[0],
                      mesh2->mNumVertices * sizeof(aiVector3D));

    std::vector<int> indices2;

    for (int i = 0; i < mesh2->mNumFaces; ++i) {
        aiFace f = mesh2->mFaces[i];
        for (int j = 0; j < f.mNumIndices; ++j) {
            indices2.push_back(f.mIndices[j]);
        }
    }

    IndexBuffer ibo2(&indices2[0], indices2.size() * sizeof(int));

    do {
        Renderer::Clear();

        program.Bind();

        glEnableVertexAttribArray(0);
        vbo1.Bind();
        glVertexAttribPointer( //
            0,                 //
            3,                 //
            GL_FLOAT,          //
            GL_FALSE,          //
            0,                 //
            (void *)0          //
        );

        ibo1.Bind();

        model1 = glm::rotate(model1, glm::radians(-1.0f), glm::vec3(-1, 1, 0));
        glm::mat4 MVP1 = camera.GetViewProjection() * model1;

        matrices.SetData(0, sizeof(glm::mat4), &MVP1[0][0]);
        data.SetData(0, sizeof(glm::vec3), &color1[0]);

        glDrawElements(GL_TRIANGLES, ibo1.GetCount(), GL_UNSIGNED_INT,
                       (void *)0);
        ibo1.Unbind();
        vbo1.Unbind();
        glDisableVertexAttribArray(0);
        //

        //
        glEnableVertexAttribArray(0);
        vbo2.Bind();
        glVertexAttribPointer( //
            0,                 //
            3,                 //
            GL_FLOAT,          //
            GL_FALSE,          //
            0,                 //
            (void *)0          //
        );

        ibo2.Bind();

        model2 = glm::rotate(model2, glm::radians(-3.0f), glm::vec3(0, 1, 0));
        glm::mat4 MVP2 = camera.GetViewProjection() * model2;

        matrices.SetData(0, sizeof(glm::mat4), &MVP2[0][0]);
        data.SetData(0, sizeof(glm::vec3), &color2[0]);

        glDrawElements(GL_TRIANGLES, ibo2.GetCount(), GL_UNSIGNED_INT,
                       (void *)0);

        ibo2.Unbind();
        vbo2.Unbind();
        glDisableVertexAttribArray(0);

        glfwSwapBuffers(window.GetWindow());
        glfwPollEvents();
    } while (!window.ShouldClose());

    glDeleteVertexArrays(1, &vertexArrayID);
}
