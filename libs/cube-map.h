
#pragma once

#include <string>
#include <vector>
#include <glad/glad.h>
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#endif

class CubeMap {

public:
    CubeMap(const std::vector<std::string> &filePaths, bool withAlpha = false);
    ~CubeMap();

    void useCubeMap(GLenum textureUnit = GL_TEXTURE0);

private:
    GLuint textureId;

    // prevent copy and copy-assignment
    CubeMap& operator=(const CubeMap &other);
    CubeMap(const CubeMap &other);

};

CubeMap::CubeMap(const std::vector<std::string> &filePaths, bool withAlpha) {

    if (filePaths.size() != 6) {
        std::cout << "Cube map requires six sides\n";
        throw;
    }

    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);

    int width, height, bitDepth;
    for (int i = 0; i < 6; i++) {

        unsigned char* textureData = stbi_load(filePaths[i].c_str(), &width, &height, &bitDepth, 0);
        if (!textureData) {
            std::cout << "Failed to load: " << filePaths[i] << "\n";
            throw;
        }

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, withAlpha ? GL_RGBA : GL_RGB, width, height, 0, 
                        withAlpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, textureData);

        stbi_image_free(textureData);

    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

}

CubeMap::~CubeMap() {
    glDeleteTextures(1, &textureId);
}

void CubeMap::useCubeMap(GLenum textureUnit) {

    glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);

}
