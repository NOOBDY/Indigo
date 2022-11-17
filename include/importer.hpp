#ifndef IMPORTER_HPP
#define IMPORTER_HPP

#include "pch.hpp"

#include "vertex_array.hpp"

/**
 * Current implementation is extremely limiting
 *
 * Only `.obj` file support and no `.mtl` file support
 *
 * Only the first UV will be loaded
 */
class Importer {
public:
    static VertexArray LoadFile(const std::string &filepath);
};

#endif