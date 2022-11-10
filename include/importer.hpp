#ifndef IMPORTER_HPP
#define IMPORTER_HPP

#include "pch.hpp"

/**
 * Current implementation is extremely limiting
 *
 * Only `.obj` file support and no `.mtl` file support
 *
 * Only the first UV will be loaded
 */
class Importer {
public:
    Importer(const std::string &filepath);

    const std::vector<float> &GetVertices() const { return m_Vertices; }
    const std::vector<float> &GetUVs() const { return m_UVs; }
    const std::vector<float> &GetNormals() const { return m_Normals; }
    const std::vector<float> &GetTangents() const { return m_Tangents; }
    const std::vector<float> &GetBitangents() const { return m_Bitangents; }

    const std::vector<unsigned int> &GetIndices() const { return m_Indices; }

private:
    std::vector<float> m_Vertices;
    std::vector<float> m_UVs;
    std::vector<float> m_Normals;
    std::vector<float> m_Tangents;
    std::vector<float> m_Bitangents;

    std::vector<unsigned int> m_Indices;
};

#endif