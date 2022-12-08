#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "pch.hpp"

#include "vertex_array.hpp"

namespace Renderer {
void Init();
/**
 * Clears the color and depth buffer
 */
void Clear();
void ClearColor(float r, float g, float b, float a);
void Draw(const unsigned int indexCount);

void EnableDepthTest();
void DisableDepthTest();

void EnableCullFace();
void DisableCullFace();

}; // namespace Renderer

#endif