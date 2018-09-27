
#pragma once

#include <tuple>
#include <unordered_map>
#include <math.h>
#include <algorithm>

#include <glm/glm.hpp>

#include "../libs/perlin.h"
#include "../libs/camera.h"
#include "../libs/aabb.h"
#include "../libs/timer.h"
#include "./chunk.h"
#include "./block.h"


// NB: As it stands, World assumes that the world is only one chunk high
// (Chunk, however, assumes it can have neighbours in any direction)

class World {

public:

    // chunks will be loaded into the world when they are 
    // within DRAW_RADIUS distance of the camera; they will 
    // be unloaded when they are outside OUTER_RADIUS of the camera
    // (this is to prevent lots of loading/unloading that would 
    // occur if there were a single barrier that was being crossed
    // a lot)
    static constexpr int DRAW_RADIUS = 16;
    static constexpr int CREATE_RADIUS = DRAW_RADIUS + 1;
    static constexpr int OUTER_RADIUS = CREATE_RADIUS + 1;

    World(): noise(1234) {

        int maxNumChunks = std::pow(OUTER_RADIUS + 1, 2);

        drawList.reserve(maxNumChunks);

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
        float offsetCameraZ = camera.getPosition().z - Chunk::CHUNK_SIZE_Z / 2.0;

        for (auto const& [key, chunk] : chunks) {
            if (camera.canSee(chunk->getAABB()) && chunk->hasMesh()) {
                const glm::vec3& chunkPos = chunk->getPosition();
                drawList.emplace_back(chunk, 
                    std::pow(chunkPos.x - offsetCameraX, 2) + std::pow(chunkPos.z - offsetCameraZ, 2));
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

private:

    struct VisibleChunk {
        VisibleChunk(Chunk* chunk, int distanceSquared): chunk(chunk), distanceSquared(distanceSquared) {}
        Chunk* chunk;
        int distanceSquared;
    };

    // in order to use std::unordered_map with std::pair as a key, we need to 
    // create our own hash function
    // https://stackoverflow.com/a/20602159/1937302
    // https://stackoverflow.com/a/27952689/1937302
    struct hashPair {
        template <typename T, typename U>
        std::size_t operator()(const std::pair<T, U> &x) const {
            std::size_t lhs = std::hash<T>()(x.first);
            std::size_t rhs = std::hash<U>()(x.second);
            lhs ^= rhs + 0x9e3779b97f4a7c15 + (lhs << 6) + (lhs >> 2);
            return lhs;
        }
    };

    std::unordered_map<std::pair<int, int>, Chunk*, hashPair> chunks;
    PerlinNoise noise;
    std::vector<VisibleChunk> drawList;

    void buildChunks(const glm::vec3 &position) {

        int currentI = std::floor(position.x / Chunk::CHUNK_SIZE_X);
        int currentJ = std::floor(position.z / Chunk::CHUNK_SIZE_Z);

        int minI = currentI - CREATE_RADIUS;
        int minJ = currentJ - CREATE_RADIUS;
        int maxI = currentI + 1 + CREATE_RADIUS;
        int maxJ = currentJ + 1 + CREATE_RADIUS;

        Timer timer{};

        // create Chunks within required area:
        for (int i = minI; i <= maxI; i++) {
                for (int j = minJ; j <= maxJ; j++) {

                    if (getChunk(i, j) != nullptr) {
                        // chunk already exists
                        continue;
                    }

                    glm::ivec3 chunkPosition = glm::ivec3(
                        i * Chunk::CHUNK_SIZE_X, 
                        0,
                        j * Chunk::CHUNK_SIZE_Z );

                    // TODO: check that chunk is actually within radius?

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
                                    chunk->blocks[x][y][z].type = Block::ROCK;
                                } else {
                                    if (worldY == topSoilHeight) {
                                        chunk->blocks[x][y][z].type = Block::GRASS;
                                    } else if (worldY < topSoilHeight) {
                                        chunk->blocks[x][y][z].type = Block::DIRT;
                                    } else if (worldY < waterLevel) {
                                        chunk->blocks[x][y][z].type = Block::WATER;
                                    } else {
                                        chunk->blocks[x][y][z].type = Block::AIR;
                                    }
                                }

                            }
                        }
                    }

                    chunks.insert({ std::make_pair(i, j), chunk });

                }
        }

        timer.printTime("world gen");
        timer.reset();

        // make sure all chunks within draw radius have a mesh:
        for (auto const& [key, chunk] : chunks) {

            if (chunk->hasMesh()) { continue; }

            if (key.first > minI && key.first < maxI &&
                key.second > minJ && key.second < maxJ) {
                
                    Chunk::Neighbourhood neighbourhood {
                        getChunk(key.first - 1, key.second), // left
                        getChunk(key.first + 1, key.second), // right
                        nullptr, // top
                        nullptr, // bottom
                        getChunk(key.first, key.second + 1), // front
                        getChunk(key.first, key.second - 1)  // back
                    };
                    chunk->initMesh(neighbourhood);

            }

        }

        timer.printTime("meshes built");

    }

    void freeChunks(const glm::vec3 &position) {

        int currentI = std::floor(position.x / Chunk::CHUNK_SIZE_X);
        int currentJ = std::floor(position.z / Chunk::CHUNK_SIZE_Z);

        int minI = currentI - OUTER_RADIUS;
        int minJ = currentJ - OUTER_RADIUS;
        int maxI = currentI + 1 + OUTER_RADIUS;
        int maxJ = currentJ + 1 + OUTER_RADIUS;

        for (auto it = chunks.cbegin(); it != chunks.cend(); ) {

            std::pair<int, int> key = it->first;
            
            if (key.first >= minI && key.first <= maxI &&
                key.second >= minJ && key.second <= maxJ) {
                
                    ++it;
                    continue;
            }

            Chunk* chunk = it->second;
            it = chunks.erase(it);
            delete chunk;

        }

    }

    Chunk* getChunk(int i, int j) const {

        auto search = chunks.find(std::make_pair(i, j));
        if (search == chunks.end()) {
            return nullptr;
        }
        return search->second;

    }

};
