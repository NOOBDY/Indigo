#include "importer.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <filesystem>

#include "model.hpp"
#include "texture.hpp"
#include "log.hpp"
#include "exception.hpp"
namespace fs = std::filesystem;

Mesh Importer::LoadFile(const std::string &filepath) {
    LOG_TRACE("Loading File: '{}'", filepath);

    Assimp::Importer importer;

    Mesh result;
    unsigned int totalVertexCount = 0;

    std::vector<unsigned int> indexBuffer;

    const aiScene *scene =
        importer.ReadFile(filepath, aiProcessPreset_TargetRealtime_Quality);

    if (scene == NULL) {
        LOG_ERROR("{}", importer.GetErrorString());
        throw FileNotFoundException(filepath);
    }

    /**
     * // There's probably a better way to predetermine vector size and
     * // load entire blocks of memory instead of `push_back()` each
     * element
     *
     * // Update 1: changed from `push_back()` to `insert()` for some
     * elements,
     * // but IDK if there are more optimizing space
     *
     * // Update 2: Currently loads only the first mesh and uses a
     * shared buffer
     * // to load mesh data into the GPU. This probably breaks OOP
     * principles
     * // but the extra memory saved is pretty substantial(~2/3 less but
     * // unverified) especially on large files.
     * // Another index buffer vector has to be used due to type
     * differences. If
     * // this problem can solved, the memory usage can be cut down to
     * ~5/6 less
     * // than the original implementation.
     *
     * Update 3: Now `Assimp` data is directly put into
     * `glNamedBufferData()` except for index buffers, reasons are
     * listed below
     */

    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        const aiMesh *mesh = scene->mMeshes[i];
        std::shared_ptr<VertexArray> va = std::make_shared<VertexArray>();

        indexBuffer.resize(sizeof(float) * 3 * mesh->mNumFaces);

        // Vertices
        va->AddVertexBuffer(std::make_shared<VertexBuffer>(
            &mesh->mVertices[0].x, 3 * mesh->mNumVertices, 3 * sizeof(float)));

        // UVs
        va->AddVertexBuffer(std::make_shared<VertexBuffer>(
            &mesh->mTextureCoords[0][0].x, 3 * mesh->mNumVertices,
            3 * sizeof(float)));

        // Normals
        va->AddVertexBuffer(std::make_shared<VertexBuffer>(
            &mesh->mNormals[0].x, 3 * mesh->mNumVertices, 3 * sizeof(float)));

        // Tangents
        va->AddVertexBuffer(std::make_shared<VertexBuffer>(
            &mesh->mTangents[0].x, 3 * mesh->mNumVertices, 3 * sizeof(float)));

        // Bitangents
        va->AddVertexBuffer(std::make_shared<VertexBuffer>(
            &mesh->mBitangents[0].x, 3 * mesh->mNumVertices,
            3 * sizeof(float)));

        // Indices
        indexBuffer.clear();
        for (unsigned int j = 0; j < mesh->mNumFaces; ++j) {
            /**
             * After some debugging, I discovered that `mIndices` under
             * `mesh->mFaces` are not on a contiguous block of memory.
             * i.e. The address of `mesh->mFaces[0].mIndices[2]` and
             * `mesh->mFaces[1].mIndices[0]` are not the same.
             * So inserting an entire block of memory into the vector is
             * not possible, at least not that I know of.
             */
            indexBuffer.insert(               //
                indexBuffer.end(),            //
                &mesh->mFaces[j].mIndices[0], //
                &mesh->mFaces[j].mIndices[3]  //
            );
        }
        va->SetIndexBuffer(std::make_shared<IndexBuffer>(indexBuffer));

        result.push_back(va);
        totalVertexCount += mesh->mNumVertices;
    }

    LOG_INFO("Loaded {} Vertices", totalVertexCount);

    return result;
}
std::shared_ptr<Model> Importer::LoadFileModel(const std::string name,
                                               const std::string &filepath) {
    LOG_TRACE("Loading File: '{}'", filepath);

    Assimp::Importer importer;

    Mesh result;
    unsigned int totalVertexCount = 0;

    std::vector<unsigned int> indexBuffer;

    const aiScene *scene =
        importer.ReadFile(filepath, aiProcessPreset_TargetRealtime_Quality);

    if (scene == NULL) {
        LOG_ERROR("{}", importer.GetErrorString());
        throw FileNotFoundException(filepath);
    }

    /**
     * // There's probably a better way to predetermine vector size and
     * // load entire blocks of memory instead of `push_back()` each element
     *
     * // Update 1: changed from `push_back()` to `insert()` for some elements,
     * // but IDK if there are more optimizing space
     *
     * // Update 2: Currently loads only the first mesh and uses a shared buffer
     * // to load mesh data into the GPU. This probably breaks OOP principles
     * // but the extra memory saved is pretty substantial(~2/3 less but
     * // unverified) especially on large files.
     * // Another index buffer vector has to be used due to type differences. If
     * // this problem can solved, the memory usage can be cut down to ~5/6 less
     * // than the original implementation.
     *
     * Update 3: Now `Assimp` data is directly put into
     * `glNamedBufferData()` except for index buffers, reasons are listed below
     */

    aiMaterial *material = nullptr;
    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        const aiMesh *mesh = scene->mMeshes[i];
        std::shared_ptr<VertexArray> va = std::make_shared<VertexArray>();

        indexBuffer.resize(sizeof(float) * 3 * mesh->mNumFaces);

        // Vertices
        va->AddVertexBuffer(std::make_shared<VertexBuffer>(
            &mesh->mVertices[0].x, 3 * mesh->mNumVertices, 3 * sizeof(float)));

        // UVs
        va->AddVertexBuffer(std::make_shared<VertexBuffer>(
            &mesh->mTextureCoords[0][0].x, 3 * mesh->mNumVertices,
            3 * sizeof(float)));

        // Normals
        va->AddVertexBuffer(std::make_shared<VertexBuffer>(
            &mesh->mNormals[0].x, 3 * mesh->mNumVertices, 3 * sizeof(float)));

        // Tangents
        va->AddVertexBuffer(std::make_shared<VertexBuffer>(
            &mesh->mTangents[0].x, 3 * mesh->mNumVertices, 3 * sizeof(float)));

        // Bitangents
        va->AddVertexBuffer(std::make_shared<VertexBuffer>(
            &mesh->mBitangents[0].x, 3 * mesh->mNumVertices,
            3 * sizeof(float)));

        // Indices
        indexBuffer.clear();
        for (unsigned int j = 0; j < mesh->mNumFaces; ++j) {
            /**
             * After some debugging, I discovered that `mIndices` under
             * `mesh->mFaces` are not on a contiguous block of memory.
             * i.e. The address of `mesh->mFaces[0].mIndices[2]` and
             * `mesh->mFaces[1].mIndices[0]` are not the same.
             * So inserting an entire block of memory into the vector is not
             * possible, at least not that I know of.
             */
            indexBuffer.insert(               //
                indexBuffer.end(),            //
                &mesh->mFaces[j].mIndices[0], //
                &mesh->mFaces[j].mIndices[3]  //
            );
        }
        va->SetIndexBuffer(std::make_shared<IndexBuffer>(indexBuffer));

        result.push_back(va);
        if (material == nullptr)
            material = scene->mMaterials[mesh->mMaterialIndex];
        totalVertexCount += mesh->mNumVertices;
    }
    // aiString fileBaseColor, fileMetallic, fileRoughness, fileName;

    // material->Get(AI_MATKEY_BASE_COLOR_TEXTURE, fileBaseColor);
    // material->Get(AI_MATKEY_METALLIC_TEXTURE, fileMetallic);
    // material->Get(AI_MATKEY_ROUGHNESS_TEXTURE, fileRoughness);
    // LOG_INFO("{}", std::string(fileName.data));

    LOG_INFO("Loaded {} Vertices", totalVertexCount);
    auto model = std::make_shared<Model>(name, result,
                                         Transform({0, 0, 0},       //
                                                   {180, 180, 180}, //
                                                   {1, 1, 1}));

    auto textures = {
        aiTextureType_BASE_COLOR,        aiTextureType_DIFFUSE,
        aiTextureType_NORMAL_CAMERA,     aiTextureType_NORMALS,
        aiTextureType_SPECULAR,          aiTextureType_SHEEN,
        aiTextureType_SHININESS,         aiTextureType_AMBIENT_OCCLUSION,
        aiTextureType_DIFFUSE_ROUGHNESS, aiTextureType_METALNESS,
        aiTextureType_EMISSION_COLOR};
    std::string path = filepath.substr(0, filepath.find_last_of("/\\"));
    for (auto textureType : textures) {
        int len = material->GetTextureCount(textureType);
        if (len == 0)
            continue;
        aiString fileName;
        material->Get(AI_MATKEY_TEXTURE(textureType, 0), fileName);
        // LOG_DEBUG("{}", fileName.data);
        auto texture =
            std::make_shared<Texture>(path + "/" + std::string(fileName.data));
        switch (textureType) {
        case aiTextureType_DIFFUSE:
            model->SetAlbedoTexture(texture);
            break;
        case aiTextureType_BASE_COLOR:
            model->SetAlbedoTexture(texture);
            break;
        case aiTextureType_NORMALS:
            model->SetNormalTexture(texture);
            break;
        case aiTextureType_NORMAL_CAMERA:
            model->SetNormalTexture(texture);
            break;
        case aiTextureType_AMBIENT_OCCLUSION:
            model->SetARMTexture(texture);
            break;
        case aiTextureType_DIFFUSE_ROUGHNESS:
            model->SetARMTexture(texture);
            break;
        case aiTextureType_SHININESS:
            model->SetARMTexture(texture);
            break;
        case aiTextureType_SPECULAR:
            model->SetARMTexture(texture);
            break;
        case aiTextureType_SHEEN:
            model->SetARMTexture(texture);
            break;
        case aiTextureType_METALNESS:
            model->SetARMTexture(texture);
            break;
        case aiTextureType_EMISSION_COLOR:
            model->SetEmissionTexture(texture);
            break;
        default:
            break;
        }
    }

    return model;
}