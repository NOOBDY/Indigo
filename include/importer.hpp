#ifndef IMPORTER_HPP
#define IMPORTER_HPP

#include "pch.hpp"

#include "vertex_array.hpp"

/**
 * Current implementation is extremely limiting
 *
 * Only `.obj` and `.glb` file support
 *
 * Limitations:
 * `.obj`:
 * - no `.mtl` file support
 * `.glb`:
 * - animation and texture are not supported and will crash the program
 *
 * Only the first mesh and the first UV will be loaded
 */
class Importer {
public:
    static VertexArray LoadFile(const std::string &filepath);
};

#endif