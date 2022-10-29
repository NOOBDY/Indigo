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

    std::vector<float> &GetVertices() { return m_Vertices; }
    std::vector<float> &GetUVs() { return m_UVs; }
    std::vector<float> &GetNormals() { return m_Normals; }
    std::vector<float> &GetTangents() { return m_Tangents; }
    std::vector<float> &GetBitangents() { return m_Bitangents; }

    std::vector<unsigned int> &GetIndices() { return m_Indices; }

private:
    std::vector<float> m_Vertices;
    std::vector<float> m_UVs;
    std::vector<float> m_Normals;
    std::vector<float> m_Tangents;
    std::vector<float> m_Bitangents;

    std::vector<unsigned int> m_Indices;
};

#endif