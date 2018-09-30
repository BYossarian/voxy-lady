
#include <iostream>
#include <string>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "./libs/window.h"
#include "./libs/camera.h"
#include "./libs/shader.h"
#include "./libs/texture-atlas.h"
#include "./libs/read-file.h"
#include "./helpers/timer.h"
#include "./helpers/frame-counter.h"
#include "./core/world.h"

int main() {
    
    Window window("Voxy Lady", 800, 600);
    window.setSwapInterval(0);

    Camera camera(glm::vec3(0, 250, 0), M_PI/2, 0, 10.0f, 0.01f, M_PI/4, window.getAspectRatio(), 0.1f, 250.0f);

    // create uniform buffer object for projection and view matrices:
    GLuint uboMatrices;
    GLuint bindingPoint = 0;
    glGenBuffers(1, &uboMatrices);
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    // allocation enough memory for the two matrices:
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
    // load data into buffer:
    glm::mat4 projection = camera.calcualateProjectionMatrix();
    glm::mat4 view = camera.calcualateViewMatrix();
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    // finally, bind buffer to GL_UNIFORM_BUFFER binding point:
    glBindBufferRange(GL_UNIFORM_BUFFER, bindingPoint, uboMatrices, 0, 2 * sizeof(glm::mat4));

    Shader blockShader(readFile("./shaders/shader-block.vs"), readFile("./shaders/shader-block.fs"));
    blockShader.useShader();
    blockShader.setUniformBufferBindingPoint("Matrices", bindingPoint);

    TextureAtlas blockTexture("./textures/atlas.png", 16, 16, true);

    World* world = new World();

    {
        Timer timer{};

        world->init(camera.getPosition());

        timer.printTime("initial world gen");
    }

    // enable depth testing & face culling:
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    double lastUpdateTime = glfwGetTime();

    FrameCounter frameCounter{};

    // render loop
    while (!window.shouldWindowClose()) {

        // handle events
        glfwPollEvents();

        // update
        double now = glfwGetTime();
        double deltaTime = now - lastUpdateTime;
        camera.update(deltaTime, window);
        lastUpdateTime = now;

        {

            Timer timer{};

            world->update(camera.getPosition());

            timer.printTime("world update");
            
        }

        // render
        glClearColor(0.55f, 0.75f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glm::vec3 lightDirection(-2.0f, -4.0f, 1.0f);
        glm::vec3 lightColour(1.0f, 1.0f, 1.0f);

        // update view in uniform buffer block:
        glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(camera.calcualateViewMatrix()));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        // draw blocks:
        blockShader.useShader();
        blockTexture.useTextureAtlas(GL_TEXTURE0);
        blockShader.setUniformInt("material.diffuseTexture", 0);
        blockShader.setUniformInt("material.shininess", 128);
        blockShader.setUniformVec3("light.direction", lightDirection);
        blockShader.setUniformVec3("light.colour",  lightColour);
        blockShader.setUniformFloat("light.ambientIntensity", 0.4f);
        blockShader.setUniformFloat("light.diffuseIntensity", 0.8f);
        blockShader.setUniformFloat("light.specularIntensity", 0.9f);
        blockShader.setUniformVec3("viewPosition", camera.getPosition());
        world->render(camera, blockShader);

        window.swapBuffers();

        frameCounter.frame();

    }

    delete world;

    std::cout << "overall fps: " << frameCounter.getTotalFPS() << std::endl;

    glfwTerminate();

    return 0;
}
