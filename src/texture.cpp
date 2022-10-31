#include "texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "log.hpp"

Texture::Texture(const int width, const int height) {
    LOG_TRACE("Creating Texture");

    glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureID);
    this->Bind();

    glTexImage2D(         //
        GL_TEXTURE_2D,    // target
        0,                // level
        GL_RGB,           // internal format
        width,            //
        height,           //
        0,                // border
        GL_RGB,           // format
        GL_UNSIGNED_BYTE, // type
        NULL              //
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    this->Unbind();
}

Texture::Texture(const std::string &textureFilepath) {
    LOG_TRACE("Creating Texture");

    glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureID);

    LoadImage(textureFilepath);
}

Texture::~Texture() {
    LOG_TRACE("Deleting Texture");
    glDeleteTextures(1, &m_TextureID);
}

void Texture::BindUnit(unsigned int slot) {
    LOG_TRACE("Binding Texture");
    glBindTextureUnit(slot, m_TextureID);
}
void Texture::Bind() {
    glBindTexture(GL_TEXTURE_2D, m_TextureID);
}
void Texture::Unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::LoadImage(const std::string &textureFilepath) {
    glBindTexture(GL_TEXTURE_2D, m_TextureID);

    stbi_set_flip_vertically_on_load(true);
    int width, height, channels;
    unsigned char *data =
        stbi_load(textureFilepath.c_str(), &width, &height, &channels, 0);

    if (data == NULL) {
        LOG_ERROR("Failed Opening File: '{}'", textureFilepath);
        throw;
    }

    glTexImage2D(                           //
        GL_TEXTURE_2D,                      // target
        0,                                  // level
        channels == 4 ? GL_RGBA8 : GL_RGB8, // internal format
        width,                              //
        height,                             //
        0,                                  // border
        channels == 4 ? GL_RGBA : GL_RGB,   // format
        GL_UNSIGNED_BYTE,                   // type
        data                                //
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);

    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
}

GLuint Texture::GetTextureLocation(const GLuint &programID,
                                   const std::string &uniformName) {
    return glGetUniformLocation(programID, uniformName.c_str());
}