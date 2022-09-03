#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <string>

#include <GL/glew.h>

#include "program.hpp"

class Texture {
public:
    Texture(const std::string &textureFilepath);
    ~Texture();

    /**
     * Bind texture to slot set in `program.SetInt()`
     */
    void Bind(unsigned int slot);

    GLuint GetTextureID() { return m_TextureID; }
    GLuint GetTextureLocation(const GLuint &programID,
                              const std::string &uniformName);

private:
    GLuint m_TextureID;
};

#endif