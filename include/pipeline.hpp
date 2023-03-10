#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#include "pch.hpp"

#include "program.hpp"

class Pipeline {
    enum Pass {
        ALBEDO,
        NORMAL,
        ARM,
        EMISSION,
        REFLECT,
        POSITION,
        DEPTH,
        LIGHTING,
        VOLUME,
        SHADOW // Must be last because cube map stuff
    };

public:
    Pipeline();

    void EnablePass(Pass pass);

private:
    std::shared_ptr<Program> m_Shadow;
    std::shared_ptr<Program> m_Basic;
    std::shared_ptr<Program> m_Light;
    std::shared_ptr<Program> m_Compositor;
};
#endif