#include "texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_write.h>

#include "log.hpp"
#include "exception.hpp"

Texture::Texture(const int width, const int height, Format format,
                 Target target, int bit)
    : m_Target(target), m_Format(format), m_Width(width), m_Height(height),
      m_Bit(bit) {
    glCreateTextures(target, 1, &m_TextureID);
    LOG_TRACE("Creating Texture {}", m_TextureID);

    Update(nullptr);

    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glGenerateMipmap(m_Target);
}

Texture::Texture(const std::string &textureFilepath, int bit)
    : m_Target(IMAGE_2D) {
    glCreateTextures(m_Target, 1, &m_TextureID);
    LOG_TRACE("Creating Texture {}", m_TextureID);

    LoadImage(textureFilepath, bit);
}

Texture::~Texture() {
    LOG_TRACE("Deleting Texture {}", m_TextureID);
    glDeleteTextures(1, &m_TextureID);
}

void Texture::Bind(unsigned int slot) {
    LOG_TRACE("Binding Texture");
    glBindTextureUnit(slot, m_TextureID);
}

void Texture::Unbind() {
    glBindTexture(m_Target, 0);
}

void Texture::SetWidth(int width) {
    m_Width = width;
    if (m_Target == Texture::Target::CUBE && m_Height != m_Width) {
        LOG_WARN("texture cubemap must have same height and width");
        LOG_WARN("force same height and width");
        m_Height = width;
    }
    Update(nullptr);
}

void Texture::SetHeight(int height) {
    m_Height = height;
    if (m_Target == Texture::Target::CUBE && m_Height != m_Width) {
        LOG_WARN("texture cubemap must have same height and width");
        LOG_WARN("force same height and width");
        m_Width = height;
    }
    Update(nullptr);
}

GLuint Texture::GetTextureLocation(const GLuint &programID,
                                   const std::string &uniformName) {
    return glGetUniformLocation(programID, uniformName.c_str());
}

std::vector<unsigned char>
Texture::GetPixelColorByPosition(glm::vec2 pos) const {
    std::vector<GLubyte> pixels = GetTexturePixels();
    std::vector<GLubyte> target;
    unsigned int pixelsSize = GetPixelSize();

    target.reserve(pixelsSize);
    int start = pixelsSize * (m_Width * pos.y + pos.x);
    target.insert(target.begin(), &pixels[start], &pixels[start + pixelsSize]);

    return target;
}

void Texture::LoadImage(const std::string &textureFilepath, int bit) {
    m_Target = IMAGE_2D;
    stbi_set_flip_vertically_on_load(true);
    int width, height, channels;
    unsigned char *data =
        stbi_load(textureFilepath.c_str(), &width, &height, &channels, 0);

    if (data == NULL) {
        throw FileNotFoundException(textureFilepath);
    }
    m_Target = IMAGE_2D;
    m_Format = Channels2Format(channels);
    m_Width = width;
    m_Height = height;
    m_Bit = bit;

    /**
     * TODO: Add support for 1 channel png files
     * The seg fault when loading the reflection(?) and AO map is caused by
     * these images only having one channel. Currently the function only loads
     * images as `GL_RGB` and `GL_RGBA` which has 3 and 4 channels, which causes
     * the program to read addresses out of bounds `glTexImage2D` documentation
     * for format table
     * https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml
     */

    Update(data);

    glTexParameteri(m_Target, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(m_Target, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(m_Target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(m_Target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glGenerateMipmap(m_Target);

    stbi_image_free(data);
}

/// @brief change format to internal format
int Texture::Format2Bit(Texture::Format inFormat, int bit) const {
    // GL_RGBA16 will be 16bit for each channel totally 64 bit
    switch (bit) {
    case 8: {
        switch (inFormat) {
        case Texture::Format::DEPTH:
            return GL_DEPTH_COMPONENT16;
        case Texture::Format::R:
            return GL_R8;
        case Texture::Format::RG:
            return GL_RG8;
        case Texture::Format::RGB:
            return GL_RGB8;
        case Texture::Format::RGBA:
            return GL_RGBA8;
        default:
            throw std::runtime_error("invalid format");
        }
    }
    case 16: {
        switch (inFormat) {
        case Texture::Format::DEPTH:
            return GL_DEPTH_COMPONENT16;
        case Texture::Format::R:
            return GL_R16F;
        case Texture::Format::RG:
            return GL_RG16F;
        case Texture::Format::RGB:
            return GL_RGB16F;
        case Texture::Format::RGBA:
            return GL_RGBA16F;
        default:
            throw std::runtime_error("invalid format");
        }
    }
    case 32: {
        switch (inFormat) {
        case Texture::Format::DEPTH:
            return GL_DEPTH_COMPONENT24;
        case Texture::Format::R:
            return GL_R32F;
        case Texture::Format::RG:
            return GL_RG32F;
        case Texture::Format::RGB:
            return GL_RGB32F;
        case Texture::Format::RGBA:
            return GL_RGBA32F;
        default:
            throw std::runtime_error("invalid format");
        }
    }
    }
    throw std::runtime_error("invalid bit number");
}

int Texture::Format2Channels(Format format) const {
    switch (format) {
    case Format::DEPTH:
        return 1;
        break;
    case Format::R:
        return 1;
        break;
    case Format::RG:
        return 2;
        break;
    case Format::RGB:
        return 3;
        break;
    case Format::RGBA:
        return 4;
        break;

    default:
        throw std::runtime_error("invalid format");
        break;
    }
}

Texture::Format Texture::Channels2Format(int channel) const {
    switch (channel) {
    case 1:
        return Texture::Format::R;
    case 2:
        return Texture::Format::RG;
    case 3:
        return Texture::Format::RGB;
    case 4:
        return Texture::Format::RGBA;
    default:
        throw std::runtime_error("Image channel unsupported");
    }
}

void Texture::SaveTexture(const std::string &path) const {
    int channelNumber = Format2Channels(m_Format);
    std::vector<unsigned char> pixels = GetTexturePixels();
    stbi_flip_vertically_on_write(1);
    // it will take about 2s to save
    LOG_INFO("saving image {}", path);
    stbi_write_png(path.c_str(), m_Width, m_Height, channelNumber,
                   pixels.data(), m_Width * channelNumber);
    LOG_INFO("saved");
}

void Texture::Update(unsigned char *data) {
    glBindTexture(m_Target, m_TextureID);
    if (m_Target == CUBE) {
        for (int i = 0; i < 6; i++)
            glTexImage2D(                           //
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, // target
                0,                                  // level
                Format2Bit(m_Format, m_Bit),        // internal format
                m_Width,                            //
                m_Height,                           //
                0,                                  // border
                m_Format,                           // format
                GL_UNSIGNED_BYTE,                   // type
                data                                //
            );

    }

    else if (m_Target == IMAGE_2D) {
        glTexImage2D(                    //
            m_Target,                    // target
            0,                           // level
            Format2Bit(m_Format, m_Bit), // internal format
            m_Width,                     //
            m_Height,                    //
            0,                           // border
            m_Format,                    // format
            GL_UNSIGNED_BYTE,            // type
            data                         //
        );
    } else {
        throw std::runtime_error("invalid target");
    }
}

std::vector<GLubyte> Texture::GetTexturePixels() const {
    std::vector<GLubyte> pixels;
    pixels.resize(GetPixelSize() * m_Width * m_Height);

    glGetTextureImage(m_TextureID, 0, m_Format, GL_UNSIGNED_BYTE,
                      GetPixelSize() * m_Width * m_Height, pixels.data());

    return pixels;
}
