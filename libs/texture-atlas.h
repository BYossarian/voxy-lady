
#pragma once

#include <string>
#include <glad/glad.h>
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#endif

class TextureAtlas {

public:
    TextureAtlas(const std::string &filePath, int width, int height, bool withAlpha = false);
    ~TextureAtlas();

    void useTextureAtlas(GLenum textureUnit = GL_TEXTURE0);

    // prevent copy and copy-assignment
    TextureAtlas& operator=(const TextureAtlas&) = delete;
    TextureAtlas(const TextureAtlas&) = delete;

private:
    GLuint textureId;
    int width, height, totalWidth, totalHeight, bitDepth;

};

TextureAtlas::TextureAtlas(const std::string &filePath, int width, int height, bool withAlpha): width(width), height(height) {

    // TODO: this doesn't need setting everytime a Texture is created, so find better place for it
    stbi_set_flip_vertically_on_load(true);

    unsigned char* textureData = stbi_load(filePath.c_str(), &totalWidth, &totalHeight, &bitDepth, 0);
    if (!textureData) {
        std::cout << "Failed to load: " << filePath << "\n";
        throw;
    }

    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D_ARRAY, textureId);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int numColumns = totalWidth / width;
    int numRows = totalHeight / height;

    // allocate storage:
    glTexImage3D(GL_TEXTURE_2D_ARRAY,
            0,                                      // minmap level
            withAlpha ? GL_RGBA : GL_RGB,           // internal format
            width, height, numColumns * numRows,    // width, height, numImages
            0,                                      // no longer used
            withAlpha ? GL_RGBA : GL_RGB,           // raw pixel format
            GL_UNSIGNED_BYTE,                       // raw pixel data type
            nullptr);                               // pointer to data

    // load individual images into storage:
    GLint initalUnpackRowLength;
    glGetIntegerv(GL_UNPACK_ROW_LENGTH, &initalUnpackRowLength);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, totalWidth);

    for (int i = 0; i < numColumns; i++) {
        for (int j = 0; j < numRows; j++) {
            glTexSubImage3D(
                GL_TEXTURE_2D_ARRAY,
                0,                              // minmap level
                // NB: because our image is flipped as it's loaded, our image array index 
                // is (numRows - j - 1) * numColumns + i rather than just j * numColumns + i
                0, 0, (numRows - j - 1) * numColumns + i,       // xoffset, yoffset, image array index
                width, height, 1,               // width, height, number of images
                withAlpha ? GL_RGBA : GL_RGB,   // raw pixel format
                GL_UNSIGNED_BYTE,               // raw pixel data type
                textureData + (j * height * totalWidth + i * width) * (withAlpha ? 4 : 3)); // pointer to data
        }
    }

    stbi_image_free(textureData);

    glPixelStorei(GL_UNPACK_ROW_LENGTH, initalUnpackRowLength);

    glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

}

TextureAtlas::~TextureAtlas() {
    glDeleteTextures(1, &textureId);
}

void TextureAtlas::useTextureAtlas(GLenum textureUnit) {

    glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_2D_ARRAY, textureId);

}
