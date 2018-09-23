
#pragma once

#include <tuple>
#include <map>
#include <math.h>
#include <algorithm>

#include <glm/glm.hpp>

#include "../libs/perlin.h"
#include "../libs/camera.h"
#include "../libs/aabb.h"
#include "./chunk.h"
#include "./block.h"

class World {

public:

    // chunks will be loaded into the world when they are 
    // within INNER_RADIUS distance of the camera; they will 
    // be unloaded when they are outside OUTER_RADIUS of the camera
    // (this is to prevent lots of loading/unloading that would 
    // occur if there were a single barrier that was being crossed
    // a lot)
    static const int INNER_RADIUS = 16;
    static const int OUTER_RADIUS = INNER_RADIUS + 1;

    World(): noise(1234) {

        drawList.reserve(std::pow(OUTER_RADIUS + 1, 2));

    }

    ~World() {

        for (auto const& [key, chunk] : chunks) {
            delete chunk;
        }

    }

    void init(const glm::vec3 &position) {

        buildChunks(position);

    }

    void update(const glm::vec3 &position) {

        freeChunks(position);
        buildChunks(position);

    }

    void render(const Camera &camera, const Shader &shader) {

        // center of the chunks will be the chunk's position + Chunk::CHUNK_SIZE_X / 2.0 etc. 
        // we can do the + Chunk::CHUNK_SIZE_X bit here to avoid repeating it for each chunk:
        float offsetCameraX = camera.getPosition().x - Chunk::CHUNK_SIZE_X / 2.0;
        float offsetCameraY = camera.getPosition().y - Chunk::CHUNK_SIZE_Y / 2.0;
        float offsetCameraZ = camera.getPosition().z - Chunk::CHUNK_SIZE_Z / 2.0;

        for (auto const& [key, chunk] : chunks) {
            if (camera.canSee(chunk->getAABB()) && chunk->hasMesh()) {
                const glm::vec3& chunkPos = chunk->getPosition();
                drawList.emplace_back(chunk, 
                    std::pow(chunkPos.x - offsetCameraX, 2) + std::pow(chunkPos.y - offsetCameraY, 2) + std::pow(chunkPos.z - offsetCameraZ, 2));
            }
        }

        std::sort(drawList.begin(), drawList.end(), [](const VisibleChunk &a, const VisibleChunk &b) {
            return a.distanceSquared < b.distanceSquared;
        });

        for (int i = 0, l = drawList.size(); i < l; i++) {
            drawList[i].chunk->render(shader);
        }

        drawList.clear();

    }

    // getting a nullptr as a return value means the chunk isn't currently loaded
    // or is outside the bounds of the world
    Chunk* getChunk(const glm::vec3& position) const {

        return getChunk(floor(position.x / Chunk::CHUNK_SIZE_X),
                        floor(position.y / Chunk::CHUNK_SIZE_Y),
                        floor(position.z / Chunk::CHUNK_SIZE_Z) );

    }

    // getting a nullptr means the block is part of a chunk that isn't loaded 
    // or is outside the bounds of the world
    Block* getBlock(const glm::vec3& position) const {

        glm::ivec3 chunkIndex = glm::ivec3(
                floor(position.x / Chunk::CHUNK_SIZE_X),
                floor(position.y / Chunk::CHUNK_SIZE_Y),
                floor(position.z / Chunk::CHUNK_SIZE_Z) );
        Chunk* chunk = getChunk(chunkIndex.x, chunkIndex.y, chunkIndex.z);
        if (chunk == nullptr) { return nullptr; }

        return &chunk->blocks[static_cast<int>(floor(position.x - chunkIndex.x * Chunk::CHUNK_SIZE_X))]
                                [static_cast<int>(floor(position.y - chunkIndex.y * Chunk::CHUNK_SIZE_Y))]
                                [static_cast<int>(floor(position.z - chunkIndex.z * Chunk::CHUNK_SIZE_Z))];

    }

private:

    struct VisibleChunk {
        VisibleChunk(Chunk* chunk, int distanceSquared): chunk(chunk), distanceSquared(distanceSquared) {}
        Chunk* chunk;
        int distanceSquared;
    };

    std::map<std::tuple<int, int, int>, Chunk*> chunks;
    PerlinNoise noise;
    std::vector<VisibleChunk> drawList;

    void buildChunks(const glm::vec3 &position) {

        int minI = static_cast<int>(floor(position.x / Chunk::CHUNK_SIZE_X)) - INNER_RADIUS;
        int minJ = 0;//static_cast<int>(floor(position.y / Chunk::CHUNK_SIZE_Y)) - INNER_RADIUS;
        int minK = static_cast<int>(floor(position.z / Chunk::CHUNK_SIZE_Z)) - INNER_RADIUS;
        int maxI = static_cast<int>(ceil(position.x / Chunk::CHUNK_SIZE_X)) + INNER_RADIUS;
        int maxJ = 1;//static_cast<int>(ceil(position.y / Chunk::CHUNK_SIZE_Y)) + INNER_RADIUS;
        int maxK = static_cast<int>(ceil(position.z / Chunk::CHUNK_SIZE_Z)) + INNER_RADIUS;

        // TODO: are these min/max right?

        for (int i = minI; i < maxI; i++) {
            for (int j = minJ; j < maxJ; j++) {
                for (int k = minK; k < maxK; k++) {

                    glm::ivec3 chunkPosition = glm::ivec3(
                        i * Chunk::CHUNK_SIZE_X, 
                        j * Chunk::CHUNK_SIZE_Y,
                        k * Chunk::CHUNK_SIZE_Z );

                    // TODO: check that chunk is actually within radius? YES

                    // TODO: would be quicker to use map.count?
                    if (getChunk(i, j, k) != nullptr) {
                        // chunk already exists
                        continue;
                    }

                    Chunk* chunk = new Chunk(chunkPosition);
                                            
                    for (int x = 0; x < Chunk::CHUNK_SIZE_X; x++) {
                        for (int z = 0; z < Chunk::CHUNK_SIZE_Z; z++) {

                            const int topSoilHeight = 200 + 4 * noise.noise(static_cast<float>(x + chunkPosition.x) / Chunk::CHUNK_SIZE_X, static_cast<float>(z + chunkPosition.z) / Chunk::CHUNK_SIZE_Z, 1) 
                                                    + 16 * noise.noise((static_cast<float>(x + chunkPosition.x) /  Chunk::CHUNK_SIZE_X) / 4, (static_cast<float>(z + chunkPosition.z) / Chunk::CHUNK_SIZE_Z) / 4, 1)
                                                    + 32 * noise.noise((static_cast<float>(x + chunkPosition.x) /  Chunk::CHUNK_SIZE_X) / 8, (static_cast<float>(z + chunkPosition.z) / Chunk::CHUNK_SIZE_Z) / 8, 1);
                            const int rockHeight = topSoilHeight - 14 + 2 * noise.noise(static_cast<float>(x + chunkPosition.x) / Chunk::CHUNK_SIZE_X, static_cast<float>(z + chunkPosition.z) / Chunk::CHUNK_SIZE_Z, 2) 
                                                    + fmax(0, 18 * noise.noise((static_cast<float>(x + chunkPosition.x) /  Chunk::CHUNK_SIZE_X), (static_cast<float>(z + chunkPosition.z) / Chunk::CHUNK_SIZE_Z), 2));
                            const int waterLevel = 219;
                            
                            for (int y = 0; y < Chunk::CHUNK_SIZE_Y; y++) {

                                int worldY = y + chunkPosition.y;

                                if (worldY <= rockHeight) {
                                    chunk->blocks[x][y][z].type = Block::Type::ROCK;
                                } else {
                                    if (worldY == topSoilHeight) {
                                        chunk->blocks[x][y][z].type = Block::Type::GRASS;
                                    } else if (worldY < topSoilHeight) {
                                        chunk->blocks[x][y][z].type = Block::Type::DIRT;
                                    } else if (worldY < waterLevel) {
                                        chunk->blocks[x][y][z].type = Block::Type::WATER;
                                    } else {
                                        chunk->blocks[x][y][z].type = Block::Type::AIR;
                                    }
                                }

                            }
                        }
                    }

                    chunks.insert(
                        std::map<std::tuple<int, int, int>, Chunk*>::value_type(std::make_tuple(i, j, k), 
                        chunk ));

                }
            }
        }

        for (auto const& [key, chunk] : chunks) {
            // TODO: chunk status...
            if (chunk->vertices.size() != 0) { continue; }
            Chunk::Neighbourhood neighbourhood {
                getChunk(std::get<0>(key) - 1, std::get<1>(key), std::get<2>(key)), // left
                getChunk(std::get<0>(key) + 1, std::get<1>(key), std::get<2>(key)), // right
                getChunk(std::get<0>(key), std::get<1>(key) + 1, std::get<2>(key)), // top
                getChunk(std::get<0>(key), std::get<1>(key) - 1, std::get<2>(key)), // bottom
                getChunk(std::get<0>(key), std::get<1>(key), std::get<2>(key) + 1), // front
                getChunk(std::get<0>(key), std::get<1>(key), std::get<2>(key) - 1)  // back
            };
            chunk->initMesh(neighbourhood);
        }

    }

    void freeChunks(const glm::vec3 &position) {

        int minI = static_cast<int>(floor(position.x / Chunk::CHUNK_SIZE_X)) - OUTER_RADIUS;
        int minJ = 0;//static_cast<int>(floor(position.y / Chunk::CHUNK_SIZE_Y)) - INNER_RADIUS;
        int minK = static_cast<int>(floor(position.z / Chunk::CHUNK_SIZE_Z)) - OUTER_RADIUS;
        int maxI = static_cast<int>(ceil(position.x / Chunk::CHUNK_SIZE_X)) + OUTER_RADIUS;
        int maxJ = 1;//static_cast<int>(ceil(position.y / Chunk::CHUNK_SIZE_Y)) + INNER_RADIUS;
        int maxK = static_cast<int>(ceil(position.z / Chunk::CHUNK_SIZE_Z)) + OUTER_RADIUS;

        // TODO: are these min/max right?

        for (auto it = chunks.cbegin(); it != chunks.cend(); ) {

            std::tuple<int, int, int> key = it->first;
            
            if (std::get<0>(key) >= minI && std::get<0>(key) <= maxI &&
                std::get<1>(key) >= minJ && std::get<1>(key) <= maxJ &&
                std::get<2>(key) >= minK && std::get<2>(key) <= maxK) {
                
                    ++it;
                    continue;
            }

            Chunk* chunk = it->second;
            it = chunks.erase(it);
            delete chunk;

        }

    }

    Chunk* getChunk(int i, int j, int k) const {

        auto search = chunks.find(std::make_tuple(i, j, k));
        if (search == chunks.end()) {
            return nullptr;
        }
        return search->second;

    }

};
