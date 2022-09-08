#include "importer.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "log.hpp"

Importer::Importer(const std::string &filepath) {
    LOG_TRACE("Loading File: '{}'", filepath);

    Assimp::Importer importer;

    const aiScene *scene =
        importer.ReadFile(filepath, aiProcessPreset_TargetRealtime_Quality);

    if (scene == NULL) {
        LOG_ERROR("{}", importer.GetErrorString());
        LOG_ERROR("Failed Loading File: '{}'", filepath);
        throw;
    }

    /*
     * There's probably a better way to predetermine vector size and
     * load entire blocks of memory instead of `push_back()` each element
     */
    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        const aiMesh *mesh = scene->mMeshes[i];

        for (unsigned int j = 0; j < mesh->mNumVertices; ++j) {
            m_Vertices.push_back(mesh->mVertices[j].x);
            m_Vertices.push_back(mesh->mVertices[j].y);
            m_Vertices.push_back(mesh->mVertices[j].z);
        }

        for (unsigned int j = 0; j < mesh->mNumVertices; ++j) {
            // hardcoded to read the first UV
            m_UVs.push_back(mesh->mTextureCoords[0][j].x);
            m_UVs.push_back(mesh->mTextureCoords[0][j].y);
        }

        for (unsigned int j = 0; j < mesh->mNumVertices; ++j) {
            m_Normals.push_back(mesh->mNormals[j].x);
            m_Normals.push_back(mesh->mNormals[j].y);
            m_Normals.push_back(mesh->mNormals[j].z);
        }

        for (unsigned int j = 0; j < mesh->mNumFaces; ++j) {
            // faces are triangulated so this can be hard coded
            m_Indices.push_back(mesh->mFaces[j].mIndices[0]);
            m_Indices.push_back(mesh->mFaces[j].mIndices[1]);
            m_Indices.push_back(mesh->mFaces[j].mIndices[2]);
        }
    }

    LOG_INFO("Loaded {} Vertices", m_Vertices.size());
}