#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "vertex_array.hpp"

class Renderer {
public:
    static void Init();
    static void ClearColor(float r, float g, float b, float a);
    static void Clear();
    static void Draw(const unsigned int indexCount);
};

#endif