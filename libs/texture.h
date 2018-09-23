
#pragma once

#include <string>
#include <glad/glad.h>
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#endif

class Texture {

public:
    Texture(const std::string &filePath, bool withAlpha = false);
    ~Texture();

    void useTexture(GLenum textureUnit = GL_TEXTURE0);

private:
    GLuint textureId;
    int width, height, bitDepth;

    // prevent copy and copy-assignment
    Texture& operator=(const Texture &other);
    Texture(const Texture &other);

};

Texture::Texture(const std::string &filePath, bool withAlpha) {

    // TODO: this doesn't need setting everytime a Texture is created, so find better place for it
    stbi_set_flip_vertically_on_load(true);

    unsigned char* textureData = stbi_load(filePath.c_str(), &width, &height, &bitDepth, 0);
    if (!textureData) {
        std::cout << "Failed to load: " << filePath << "\n";
        throw;
    }

    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, withAlpha ? GL_RGBA : GL_RGB, width, height, 0, withAlpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, textureData);

    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(textureData);

}

Texture::~Texture() {
    glDeleteTextures(1, &textureId);
}

void Texture::useTexture(GLenum textureUnit) {

    glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_2D, textureId);

}
