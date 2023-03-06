#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include "pch.hpp"

#include "log.hpp"
#include "program.hpp"

class Texture {
public:
    /// @brief image format channel of image RGB or RGBA ....
    enum Format {
        DEPTH = GL_DEPTH_COMPONENT,
        R = GL_RED,
        RG = GL_RG,
        RGB = GL_RGB,
        RGBA = GL_RGBA,

    };

    /// @brief image target like GL_TEXTURE_2D or CUBE map
    enum Target {
        CUBE = GL_TEXTURE_CUBE_MAP,
        IMAGE_2D = GL_TEXTURE_2D,
    };
    Texture(const int width, const int height, Format type,
            Target format = IMAGE_2D, int bit = 16);
    Texture(const std::string &textureFilepath, int bit = 16);
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
    void LoadImage(const std::string &textureFilepath, int bit);
    /// @brief change format to internal format
    constexpr const int Format2Bit(Format inFormat, int bit) {
        // GL_RGBA16 will be 16bit for each channel totally 64 bit
        switch (bit) {
        case 8: {
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
            default:
                throw std::runtime_error("invalid format");
            }
        }
        case 16: {
            switch (inFormat) {
            case Format::DEPTH:
                return GL_DEPTH_COMPONENT16;
            case Format::R:
                return GL_R16F;
            case Format::RG:
                return GL_RG16F;
            case Format::RGB:
                return GL_RGB16F;
            case Format::RGBA:
                return GL_RGBA16F;
            default:
                throw std::runtime_error("invalid format");
            }
        }
        case 32: {
            switch (inFormat) {
            case Format::DEPTH:
                return GL_DEPTH_COMPONENT24;
            case Format::R:
                return GL_R32F;
            case Format::RG:
                return GL_RG32F;
            case Format::RGB:
                return GL_RGB32F;
            case Format::RGBA:
                return GL_RGBA32F;
            default:
                throw std::runtime_error("invalid format");
            }
        }
        }
        throw std::runtime_error("invalid bit number");
        // return GL_RGB8;
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