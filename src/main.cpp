#include <vector>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "log.hpp"
#include "window.hpp"
#include "renderer.hpp"
#include "program.hpp"
#include "camera.hpp"
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

    Assimp::Importer importer;

    // begin model 1
    glm::mat4 model1 = glm::mat4(1.0f);
    glm::vec3 color1(0.8f, 0.5f, 0.0f);
    model1 = glm::translate(model1, glm::vec3(2, 0, 0));

    const aiScene *scene1 = importer.ReadFile(
        "../assets/donut.obj", aiProcessPreset_TargetRealtime_Fast);

    const aiMesh *mesh1 = scene1->mMeshes[0];

    std::vector<float> uvs1;

    for (unsigned int i = 0; i < mesh1->mNumVertices; ++i) {
        uvs1.push_back(mesh1->mTextureCoords[0][i].x);
        uvs1.push_back(mesh1->mTextureCoords[0][i].y);
    }

    std::vector<unsigned int> indices1;

    for (unsigned int i = 0; i < mesh1->mNumFaces; ++i) {
        aiFace f = mesh1->mFaces[i];
        for (unsigned int j = 0; j < f.mNumIndices; ++j) {
            indices1.push_back(f.mIndices[j]);
        }
    }

    std::shared_ptr<VertexBuffer> vbo1(std::make_shared<VertexBuffer>(
        (float *)&mesh1->mVertices[0], mesh1->mNumVertices,
        sizeof(aiVector3D)));

    std::shared_ptr<VertexBuffer> uv1(std::make_shared<VertexBuffer>(
        (float *)&uvs1[0], mesh1->mNumVertices, sizeof(aiVector2D)));

    std::shared_ptr<IndexBuffer> ibo1(
        std::make_shared<IndexBuffer>(&indices1[0], indices1.size()));
    // end model 1

    // begin model 2
    glm::mat4 model2 = glm::mat4(1.0f);
    glm::vec3 color2(0.0f, 0.8f, 0.8f);
    model2 = glm::translate(model2, glm::vec3(-2, 0, 0));

    const aiScene *scene2 = importer.ReadFile(
        "../assets/suzanne.obj", aiProcessPreset_TargetRealtime_Fast);

    const aiMesh *mesh2 = scene2->mMeshes[0];

    std::vector<float> uvs2;

    for (unsigned int i = 0; i < mesh2->mNumVertices; ++i) {
        uvs2.push_back(mesh2->mTextureCoords[0][i].x);
        uvs2.push_back(mesh2->mTextureCoords[0][i].y);
    }

    std::vector<unsigned int> indices2;

    for (unsigned int i = 0; i < mesh2->mNumFaces; ++i) {
        aiFace f = mesh2->mFaces[i];
        for (unsigned int j = 0; j < f.mNumIndices; ++j) {
            indices2.push_back(f.mIndices[j]);
        }
    }

    std::shared_ptr<VertexBuffer> vbo2(std::make_shared<VertexBuffer>(
        (float *)&mesh2->mVertices[0], mesh2->mNumVertices,
        sizeof(aiVector3D)));

    std::shared_ptr<VertexBuffer> uv2(std::make_shared<VertexBuffer>(
        (float *)&uvs2[0], mesh2->mNumVertices, sizeof(aiVector2D)));

    std::shared_ptr<IndexBuffer> ibo2(
        std::make_shared<IndexBuffer>(&indices2[0], indices2.size()));
    // end model 2

    Texture tex1("../assets/fabric.png");
    Texture tex2("../assets/uv.png");

    program.Bind();

    program.SetInt("texture1", 0);
    program.SetInt("texture2", 1);

    glm::vec3 pos = camera.GetPosition();

    do {
        Renderer::Clear();

        if (window.GetMouseButton(GLFW_MOUSE_BUTTON_MIDDLE)) {
            glm::mat4 cameraMat =
                glm::rotate(glm::mat4(1.0f),
                            window.GetCursorDelta().x * -2 / window.GetWidth(),
                            glm::vec3(0, 1, 0)) *
                glm::translate(glm::mat4(1.0f), pos);

            pos = glm::vec3(cameraMat[3]);
        }

        camera.SetPosition(pos);
        camera.SetDirection(pos * -1.0f);
        camera.UpdateView();

        //
        VertexArray vao1;
        vao1.AddVertexBuffer(vbo1);
        vao1.AddVertexBuffer(uv1);
        vao1.SetIndexBuffer(ibo1);

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
        vao2.AddVertexBuffer(vbo2);
        vao2.AddVertexBuffer(uv2);
        vao2.SetIndexBuffer(ibo2);

        model2 = glm::rotate(model2, glm::radians(-1.0f), glm::vec3(0, 1, 0));
        glm::mat4 MVP2 = camera.GetViewProjection() * model2;

        matrices.SetData(0, sizeof(glm::mat4), &MVP2[0][0]);
        data.SetData(0, sizeof(glm::vec3), &color2[0]);

        tex2.Bind(0);
        tex1.Bind(1);

        glDrawElements(GL_TRIANGLES, vao2.GetIndexBuffer()->GetCount(),
                       GL_UNSIGNED_INT, (void *)0);

        window.UpdateCursorPosition();
        glfwSwapBuffers(window.GetWindow());
        glfwPollEvents();
    } while (!window.ShouldClose());
}
