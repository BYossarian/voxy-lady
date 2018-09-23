
#pragma once

#include <vector>
#include <functional>

#include <glm/glm.hpp>

#include "../libs/shader.h"
#include "../libs/perlin.h"
#include "../libs/aabb.h"
#include "./block.h"

const int FLOATS_PER_VERTEX = 9;
const int FLOATS_PER_FACE = 54;

// front, back, etc. defined with forward as -z, up as +y and right as +x
const float front[] = {
    // positions         // normals           // texture coords
     0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f,
     1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f, 0.0f,
     0.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,

     0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f,
     1.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f, 0.0f,
     1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f, 0.0f
};

const float back[] = {
    // positions         // normals           // texture coords
     1.0f,  0.0f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,
     0.0f,  1.0f,  0.0f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f, 0.0f,
     1.0f,  1.0f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,

     1.0f,  0.0f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,
     0.0f,  0.0f,  0.0f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f, 0.0f,
     0.0f,  1.0f,  0.0f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f, 0.0f
};

const float top[] = {
    // positions         // normals           // texture coords
     0.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
     1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f, 0.0f,
     0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,

     0.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
     1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f, 0.0f,
     1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f, 0.0f
};

const float bottom[] = {
    // positions         // normals           // texture coords
     1.0f,  0.0f,  1.0f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
     0.0f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f, 0.0f,
     1.0f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,

     1.0f,  0.0f,  1.0f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
     0.0f,  0.0f,  1.0f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f, 0.0f,
     0.0f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f, 0.0f
};

const float left[] = {
    // positions         // normals           // texture coords
     0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f, 0.0f,
     0.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f,
     0.0f,  1.0f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f, 0.0f,

     0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f, 0.0f,
     0.0f,  0.0f,  1.0f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,
     0.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f
};

const float right[] = {
    // positions         // normals           // texture coords
     1.0f,  0.0f,  1.0f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f, 0.0f,
     1.0f,  1.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f,
     1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f, 0.0f,

     1.0f,  0.0f,  1.0f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f, 0.0f,
     1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,
     1.0f,  1.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f
};

class Chunk {

friend class World;

public:

    struct Neighbourhood {
        Chunk* left;
        Chunk* right;
        Chunk* top;
        Chunk* bottom;
        Chunk* front;
        Chunk* back;
    };

    static const int CHUNK_SIZE_X = 16;
    static const int CHUNK_SIZE_Y = 256;
    static const int CHUNK_SIZE_Z = 16;

    Chunk(glm::ivec3 position): 
        position(position), 
        boundingBox{ 
            static_cast<float>(position.x), 
            static_cast<float>(position.x + CHUNK_SIZE_X),
            static_cast<float>(position.y),
            static_cast<float>(position.y + CHUNK_SIZE_Y),
            static_cast<float>(position.z),
            static_cast<float>(position.z + CHUNK_SIZE_Z)
        } {

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

    }
    ~Chunk() {

        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);

    }

    const glm::ivec3& getPosition() const { return position; };

    const AABB& getAABB() const { return boundingBox; };

    void initMesh(const Neighbourhood& neighbourhood) {

        vertices.reserve(overestimateFaces() * FLOATS_PER_FACE);

        buildMesh(neighbourhood);

        glBindVertexArray(VAO);

        // load vertex data in VBO:
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, FLOATS_PER_VERTEX * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // normal attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, FLOATS_PER_VERTEX * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        // texture coords
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, FLOATS_PER_VERTEX * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

    }

    void render(const Shader &shader) const {

        if (vertices.size() == 0) { return; }

        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(position));
        shader.setUniformMat4("model", model);
        shader.setUniformMat3("normalMatrix", glm::mat3(glm::transpose(glm::inverse(model))));

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, vertices.size() / FLOATS_PER_VERTEX);

    }

    bool hasMesh() const {
        return vertices.size() != 0;
    }

private:

    std::vector<float> vertices;
    Block blocks[CHUNK_SIZE_X][CHUNK_SIZE_Y][CHUNK_SIZE_Z];
    GLuint VBO, VAO;
    glm::ivec3 position;
    AABB boundingBox;

    // this will over-estimate the number of faces (it assumes that any chunk <-> boundary will require a face)
    // but with the result that it's much quicker to run
    int overestimateFaces() const {

        int numFaces = 0;

        for (int x = 0; x < CHUNK_SIZE_X; x++) {
            for (int y = 0; y < CHUNK_SIZE_Y; y++) {
                for (int z = 0; z < CHUNK_SIZE_Z; z++) {
                    if (!blocks[x][y][z].getProperties().visible) {
                        continue;
                    }
                    if (x == 0 || !blocks[x-1][y][z].getProperties().visible) {
                        numFaces++;
                    }
                    if (x == CHUNK_SIZE_X - 1 || !blocks[x+1][y][z].getProperties().visible) {
                        numFaces++;
                    }
                    if (y == 0 || !blocks[x][y-1][z].getProperties().visible) {
                        numFaces++;
                    }
                    if (y == CHUNK_SIZE_Y - 1 || !blocks[x][y+1][z].getProperties().visible) {
                        numFaces++;
                    }
                    if (z == 0 || !blocks[x][y][z-1].getProperties().visible) {
                        numFaces++;
                    }
                    if (z == CHUNK_SIZE_Z - 1 || !blocks[x][y][z+1].getProperties().visible) {
                        numFaces++;
                    }
                }
            }
        }

        return numFaces;

    }

    void addFace(const float* face, int x, int y, int z, int texture) {

        int startingSize = vertices.size();
        vertices.insert(vertices.end(), face, face + FLOATS_PER_FACE);
        for (int i = 0; i < FLOATS_PER_FACE; i += FLOATS_PER_VERTEX) {
            // shift vertices to correct positions (relative to chunk):
            vertices[startingSize + i] += x;
            vertices[startingSize + 1 + i] += y;
            vertices[startingSize + 2 + i] += z;
            // set correct index into texture atlas/array:
            vertices[startingSize + 8 + i] = texture;
        }

    }

    void buildMesh(const Neighbourhood& neighbourhood) {

        // REVIEW: get block properties via array for better performance?

        for (int x = 0; x < CHUNK_SIZE_X; x++) {
            for (int y = 0; y < CHUNK_SIZE_Y; y++) {
                for (int z = 0; z < CHUNK_SIZE_Z; z++) {

                    const Block::Properties &block = blocks[x][y][z].getProperties();

                    if (!block.visible) { continue; }

                    if (x == 0 && neighbourhood.left == nullptr) {
                        addFace(left, x, y, z, block.leftTexture);
                    } else {
                        Block leftNeighbour = (x == 0 ? neighbourhood.left->blocks[CHUNK_SIZE_X-1][y][z] : blocks[x-1][y][z]);
                        if (!leftNeighbour.getProperties().visible) {
                            addFace(left, x, y, z, block.leftTexture);
                        }
                    }

                    if (x == CHUNK_SIZE_X - 1 && neighbourhood.right == nullptr) {
                        addFace(right, x, y, z, block.rightTexture);
                    } else {
                        Block rightNeighbour = (x == CHUNK_SIZE_X - 1 ? neighbourhood.right->blocks[0][y][z] : blocks[x+1][y][z]);
                        if (!rightNeighbour.getProperties().visible) {
                            addFace(right, x, y, z, block.rightTexture);
                        }
                    }

                    if (y == 0 && neighbourhood.bottom == nullptr) {
                        addFace(bottom, x, y, z, block.bottomTexture);
                    } else {
                        Block bottomNeighbour = (y == 0 ? neighbourhood.bottom->blocks[x][CHUNK_SIZE_Y-1][z] : blocks[x][y-1][z]);
                        if (!bottomNeighbour.getProperties().visible) {
                            addFace(bottom, x, y, z, block.bottomTexture);
                        }
                    }

                    if (y == CHUNK_SIZE_Y - 1 && neighbourhood.top == nullptr) {
                        addFace(top, x, y, z, block.topTexture);
                    } else {
                        Block topNeighbour = (y == CHUNK_SIZE_Y - 1 ? neighbourhood.top->blocks[x][0][z] : blocks[x][y+1][z]);
                        if (!topNeighbour.getProperties().visible) {
                            addFace(top, x, y, z, block.topTexture);
                        }
                    }

                    if (z == 0 && neighbourhood.back == nullptr) {
                        addFace(back, x, y, z, block.backTexture);
                    } else {
                        Block backNeighbour = (z == 0 ? neighbourhood.back->blocks[x][y][CHUNK_SIZE_Z-1] : blocks[x][y][z-1]);
                        if (!backNeighbour.getProperties().visible) {
                            addFace(back, x, y, z, block.backTexture);
                        }
                    }

                    if (z == CHUNK_SIZE_Z-1 && neighbourhood.front == nullptr) {
                        addFace(front, x, y, z, block.frontTexture);
                    } else {
                        Block frontNeighbour = (z == CHUNK_SIZE_Z - 1 ? neighbourhood.front->blocks[x][y][0] : blocks[x][y][z+1]);
                        if (!frontNeighbour.getProperties().visible) {
                            addFace(front, x, y, z, block.frontTexture);
                        }
                    }

                }
            }
        }

    }

};
