#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "pch.hpp"

#include "vertex_array.hpp"

class Renderer {
public:
    static void Init();
    /**
     * Clears the color buffer the depth buffer
     */
    static void Clear();
    static void ClearColor(float r, float g, float b, float a);
    static void Draw(const unsigned int indexCount);

    static void EnableDepthTest() { glEnable(GL_DEPTH_TEST); }
    static void DisableDepthTest() { glDisable(GL_DEPTH_TEST); }
};

#endif