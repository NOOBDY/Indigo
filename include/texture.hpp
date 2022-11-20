#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include "pch.hpp"

#include "program.hpp"

class Texture {
public:
    enum textureType:int{
        DEPTH=GL_DEPTH_COMPONENT,
        COLOR=GL_RGB,
    };
    enum textureFormat:int{
        CUBE=GL_TEXTURE_CUBE_MAP,
        TEXTURE=GL_TEXTURE_2D,
    };
    Texture(const int width, const int height,textureType type,textureFormat format=textureFormat::TEXTURE);
    Texture(const std::string &textureFilepath);
    ~Texture();

    /**
     * Bind texture to slot set in `program.SetInt()`
     */
    void Bind(unsigned int slot);
    void Unbind();

    void SetData();

    GLuint GetTextureID() { return m_TextureID; }
    GLuint GetTextureLocation(const GLuint &programID,
                              const std::string &uniformName);

private:
    void LoadImage(const std::string &textureFilepath);

private:
    GLuint m_TextureID;
    textureFormat m_Format;
    textureType m_Type;
};

#endif