#include "texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "log.hpp"

Texture::Texture(const int width, const int height, Format format,
                 Target target)
    : m_Target(target) {
    glCreateTextures(target, 1, &m_TextureID);
    LOG_TRACE("Creating Texture {}", m_TextureID);

    glBindTexture(target, m_TextureID);

    if (target == CUBE)
        for (int i = 0; i < 6; i++)
            glTexImage2D(                           //
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, // target
                0,                                  // level
                format,                             // internal format
                width,                              //
                height,                             //
                0,                                  // border
                format,                             // format
                GL_FLOAT,                           // type
                NULL                                //
            );

    else
        glTexImage2D( //
            target,   // target
            0,        // level
            format,   // internal format
            width,    //
            height,   //
            0,        // border
            format,   // format
            GL_FLOAT, // type
            NULL      //
        );

    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

Texture::Texture(const std::string &textureFilepath) : m_Target(IMAGE_2D) {
    glCreateTextures(m_Target, 1, &m_TextureID);
    LOG_TRACE("Creating Texture {}", m_TextureID);

    LoadImage(textureFilepath);
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

void Texture::LoadImage(const std::string &textureFilepath) {
    m_Target = IMAGE_2D;
    glBindTexture(m_Target, m_TextureID);
    stbi_set_flip_vertically_on_load(true);
    int width, height, channels;
    unsigned char *data =
        stbi_load(textureFilepath.c_str(), &width, &height, &channels, 0);

    if (data == NULL) {
        LOG_ERROR("Failed Opening File: '{}'", textureFilepath);
        throw;
    }

    if (channels != 3 && channels != 4) {
        LOG_ERROR("Only 3 and 4 Channel Images are Supported");
        throw;
    }

    /**
     * TODO: Add support for 1 channel png files
     * The seg fault when loading the reflection(?) and AO map is caused by
     * these images only having one channel. Currently the function only loads
     * images as `GL_RGB` and `GL_RGBA` which has 3 and 4 channels, which causes
     * the program to read addresses out of bounds `glTexImage2D` documentation
     * for format table
     * https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml
     */
    glTexImage2D(                           //
        m_Target,                           // target
        0,                                  // level
        channels == 4 ? GL_RGBA8 : GL_RGB8, // internal format
        width,                              //
        height,                             //
        0,                                  // border
        channels == 4 ? GL_RGBA : GL_RGB,   // format
        GL_UNSIGNED_BYTE,                   // type
        data                                //
    );

    glTexParameteri(m_Target, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(m_Target, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(m_Target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(m_Target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glGenerateMipmap(m_Target);

    stbi_image_free(data);
}

GLuint Texture::GetTextureLocation(const GLuint &programID,
                                   const std::string &uniformName) {
    return glGetUniformLocation(programID, uniformName.c_str());
}