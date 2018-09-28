
#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "./cube-map.h"
#include "./shader.h"

namespace {

    std::string vertexShader = R"(
        #version 330 core

        uniform mat4 view;
        uniform mat4 projection;

        layout (location = 0) in vec3 aPos;

        out vec3 textureCoords;

        void main() {
            textureCoords = aPos;
            vec4 pos = projection * view * vec4(aPos, 1.0);
            gl_Position = pos.xyww;
        }
    )";

    std::string fragmentShader = R"(
        #version 330 core

        uniform samplerCube skybox;

        in vec3 textureCoords;

        out vec4 colour;

        void main() {    
            colour = texture(skybox, textureCoords);
        }
    )";

    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

}

class SkyBox {

public:

    SkyBox(const std::vector<std::string> &filePaths, const glm::mat4 &projection);
    ~SkyBox();

    void render(const glm::mat4 &view);

    // prevent copy and copy-assignment
    SkyBox& operator=(const SkyBox&) = delete;
    SkyBox(const SkyBox&) = delete;

private:

    Shader shader;
    CubeMap cubemap;
    GLuint skyboxVBO, skyboxVAO;

};

SkyBox::SkyBox(const std::vector<std::string> &filePaths, const glm::mat4 &projection)
    : shader(vertexShader, fragmentShader), cubemap(filePaths) {

    shader.useShader();
    shader.setUniformMat4("projection", projection);

    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);

    glBindVertexArray(skyboxVAO);

    // load vertex data into VBO:
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}
SkyBox::~SkyBox() {

    glDeleteBuffers(1, &skyboxVBO);
    glDeleteVertexArrays(1, &skyboxVAO);

}

void SkyBox::render(const glm::mat4 &view) {

    shader.useShader();
    // want the view without translation:
    shader.setUniformMat4("view", glm::mat4(glm::mat3(view)));
    cubemap.useCubeMap(GL_TEXTURE0);
    shader.setUniformInt("skybox", 0);
    glBindVertexArray(skyboxVAO);
    glDepthFunc(GL_LEQUAL);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthFunc(GL_LESS);

}
