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
    Texture(const int width, const int height, Format format,
            Target target = IMAGE_2D, int bit = 8);
    Texture(const std::string &textureFilepath, int bit = 16);
    ~Texture();

    /**
     * Bind texture to slot set in `program.SetInt()`
     */
    void Bind(unsigned int slot);
    void Unbind();

    void SetWidth(int width);
    void SetHeight(int height);

    GLuint GetTextureID() { return m_TextureID; }
    GLuint GetTextureLocation(const GLuint &programID,
                              const std::string &uniformName);
    GLuint GetTextureTarget() { return m_Target; }
    GLuint GetTextureFormat() { return m_Format; }
    int GetWidth() { return m_Width; }
    int GetHeight() { return m_Height; }
    // Get size of each pixel accounting for channels and bit count
    int GetPixelSize() const { return Format2Channels(m_Format) * (m_Bit / 8); }
    std::vector<unsigned char> GetPixelColorByPosition(glm::vec2 pos) const;

    void SaveTexture(const std::string &path) const;

private:
    void LoadImage(const std::string &textureFilepath, int bit);
    int Format2Bit(Format format, int bit) const;
    int Format2Channels(Format format) const;
    Format Channels2Format(int channel) const;
    void Update(unsigned char *data);
    std::vector<unsigned char> GetTexturePixels() const;

private:
    GLuint m_TextureID;
    Target m_Target;
    Format m_Format;
    int m_Width;
    int m_Height;
    int m_Bit;
};

#endif
