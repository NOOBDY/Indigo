#include "importer.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "log.hpp"

VertexArray Importer::LoadFile(const std::string &filepath) {
    LOG_TRACE("Loading File: '{}'", filepath);

    VertexArray va;
    Assimp::Importer importer;

    std::vector<float> sharedBuffer;
    std::vector<unsigned int> indexBuffer;

    const aiScene *scene =
        importer.ReadFile(filepath, aiProcessPreset_TargetRealtime_Quality);

    if (scene == NULL) {
        LOG_ERROR("{}", importer.GetErrorString());
        LOG_ERROR("Failed Loading File: '{}'", filepath);
        throw;
    }

    /**
     * // There's probably a better way to predetermine vector size and
     * // load entire blocks of memory instead of `push_back()` each element
     *
     * ? Update: changed from `push_back()` to `insert()` for some elements, but
     * ? IDK if there are more optimizing space
     */

    // ! hard-coded to read only the first mesh
    const aiMesh *mesh = scene->mMeshes[0];

    sharedBuffer.reserve(sizeof(float) * 3 * mesh->mNumVertices);
    indexBuffer.reserve(sizeof(float) * mesh->mFaces[0].mNumIndices *
                        mesh->mNumFaces);

    // Vertices
    sharedBuffer.insert(sharedBuffer.begin(), &mesh->mVertices[0].x,
                        &mesh->mVertices[0].x + 3 * mesh->mNumVertices);
    va.AddVertexBuffer(
        std::make_shared<VertexBuffer>(sharedBuffer, 3 * sizeof(float)));

    // UVs
    sharedBuffer.clear();
    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
        // hardcoded to read the first 2D UV
        sharedBuffer.insert(sharedBuffer.end(), &mesh->mTextureCoords[0][i].x,
                            &mesh->mTextureCoords[0][i].x + 2);
    }
    va.AddVertexBuffer(
        std::make_shared<VertexBuffer>(sharedBuffer, 2 * sizeof(float)));

    // Normals
    sharedBuffer.insert(sharedBuffer.begin(), &mesh->mNormals[0].x,
                        &mesh->mNormals[0].x + 3 * mesh->mNumVertices);
    va.AddVertexBuffer(
        std::make_shared<VertexBuffer>(sharedBuffer, 3 * sizeof(float)));

    // Tangents
    sharedBuffer.insert(sharedBuffer.begin(), &mesh->mTangents[0].x,
                        &mesh->mTangents[0].x + 3 * mesh->mNumVertices);
    va.AddVertexBuffer(
        std::make_shared<VertexBuffer>(sharedBuffer, 3 * sizeof(float)));

    // Bitangents
    sharedBuffer.insert(sharedBuffer.begin(), &mesh->mBitangents[0].x,
                        &mesh->mBitangents[0].x + 3 * mesh->mNumVertices);
    va.AddVertexBuffer(
        std::make_shared<VertexBuffer>(sharedBuffer, 3 * sizeof(float)));

    // Indices
    indexBuffer.clear();
    for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
        /**
         * After some debugging, I discovered that `mIndices` under
         * `mesh->mFaces` are not on a contiguous block of memory.
         * i.e. The address of `mesh->mFaces[0].mIndices[2]` and
         * `mesh->mFaces[1].mIndices[0]` are not the same.
         * So inserting an entire block of memory into the vector is not
         * possible, at least not that I know of.
         */
        indexBuffer.insert(indexBuffer.end(), &mesh->mFaces[i].mIndices[0],
                           &mesh->mFaces[i].mIndices[0] +
                               mesh->mFaces[i].mNumIndices);
    }
    va.SetIndexBuffer(std::make_shared<IndexBuffer>(indexBuffer));

    LOG_INFO("Loaded {} Vertices", mesh->mNumVertices);

    return va;
}