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

    /**
     * // There's probably a better way to predetermine vector size and
     * // load entire blocks of memory instead of `push_back()` each element
     *
     * ? Update: changed from `push_back()` to `insert()` for some elements, but
     * ? IDK if there are more optimizing space
     */
    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        const aiMesh *mesh = scene->mMeshes[i];

        m_Vertices.insert(m_Vertices.end(), &mesh->mVertices[0].x,
                          &mesh->mVertices[0].x + 3 * mesh->mNumVertices);

        for (unsigned int j = 0; j < mesh->mNumVertices; ++j) {
            // hardcoded to read the first 2D UV
            m_UVs.insert(m_UVs.end(), &mesh->mTextureCoords[0][j].x,
                         &mesh->mTextureCoords[0][j].x + 2);
        }

        m_Normals.insert(m_Normals.end(), &mesh->mNormals[0].x,
                         &mesh->mNormals[0].x + 3 * mesh->mNumVertices);

        m_Tangents.insert(m_Tangents.end(), &mesh->mTangents[0].x,
                          &mesh->mTangents[0].x + 3 * mesh->mNumVertices);

        m_Bitangents.insert(m_Bitangents.end(), &mesh->mBitangents[0].x,
                            &mesh->mBitangents[0].x + 3 * mesh->mNumVertices);

        for (unsigned int j = 0; j < mesh->mNumFaces; ++j) {
            /**
             * After some debugging, I discovered that `mIndices` under
             * `mesh->mFaces` are not on a contiguous block of memory.
             * i.e. The address of `mesh->mFaces[0].mIndices[2]` and
             * `mesh->mFaces[1].mIndices[0]` are not the same.
             * So inserting an entire block of memory into the vector is not
             * possible, at least not that I know of.
             */
            m_Indices.insert(m_Indices.end(), &mesh->mFaces[j].mIndices[0],
                             &mesh->mFaces[j].mIndices[0] +
                                 mesh->mFaces[j].mNumIndices);
        }
    }

    LOG_INFO("Loaded {} Vertices", m_Vertices.size());
}