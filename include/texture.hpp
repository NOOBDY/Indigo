#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include "pch.hpp"

#include "log.hpp"
#include "program.hpp"

class Texture {
public:
    enum Format {
        DEPTH = GL_DEPTH_COMPONENT,
        R = GL_RED,
        RG = GL_RG,
        RGB = GL_RGB,
        RGBA = GL_RGBA,

    };

    enum Target {
        CUBE = GL_TEXTURE_CUBE_MAP,
        IMAGE_2D = GL_TEXTURE_2D,
    };
    Texture(const int width, const int height, Format type,
            Target format = IMAGE_2D);
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
    constexpr const int Format2Bit(Format inFormat) {
        switch (inFormat) {
        case Format::DEPTH:
            return GL_DEPTH_COMPONENT16;
        case Format::R:
            return GL_R8;
        case Format::RG:
            return GL_RG8;
        case Format::RGB:
            return GL_RGB8;
        case Format::RGBA:
            return GL_RGBA8;
        }
        return GL_RGB8;
    }
    constexpr const Format Channels2Format(int channel) {
        switch (channel) {
        case 1:
            return Format::R;
        case 2:
            return Format::RG;
        case 3:
            return Format::RGB;
        case 4:
            return Format::RGBA;
        }
        LOG_ERROR("image channel {} unsupport", channel);
        throw;
    }

private:
    GLuint m_TextureID;
    Target m_Target;
    Format m_Format;
    int m_Width;
    int m_Height;
};

#endif