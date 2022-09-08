#ifndef RENDERER_HPP
#define RENDERER_HPP

class Renderer {
public:
    static void Init();
    static void ClearColor(float r, float g, float b, float a);
    static void Clear();
};

#endif