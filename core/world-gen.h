
#pragma once

#include <glm/glm.hpp>

#include "../libs/perlin.h"
#include "./block.h"

template <int CHUNK_SIZE_X, int CHUNK_SIZE_Y, int CHUNK_SIZE_Z>
class WorldGen {

public:

    WorldGen(): noise(1234) {}

    // NB: this is thread-safe
    void operator()(const glm::ivec3 &chunkPosition, Block (&blocks)[CHUNK_SIZE_X][CHUNK_SIZE_Y][CHUNK_SIZE_Z]) const {

        for (int x = 0; x < CHUNK_SIZE_X; x++) {
            for (int z = 0; z < CHUNK_SIZE_Z; z++) {

                const int topSoilHeight = 200 + 4 * noise.noise(static_cast<float>(x + chunkPosition.x) / CHUNK_SIZE_X, static_cast<float>(z + chunkPosition.z) / CHUNK_SIZE_Z, 1) 
                                        + 16 * noise.noise((static_cast<float>(x + chunkPosition.x) /  CHUNK_SIZE_X) / 4, (static_cast<float>(z + chunkPosition.z) / CHUNK_SIZE_Z) / 4, 1)
                                        + 32 * noise.noise((static_cast<float>(x + chunkPosition.x) /  CHUNK_SIZE_X) / 8, (static_cast<float>(z + chunkPosition.z) / CHUNK_SIZE_Z) / 8, 1);
                const int rockHeight = topSoilHeight - 14 + 2 * noise.noise(static_cast<float>(x + chunkPosition.x) / CHUNK_SIZE_X, static_cast<float>(z + chunkPosition.z) / CHUNK_SIZE_Z, 2) 
                                        + fmax(0, 18 * noise.noise((static_cast<float>(x + chunkPosition.x) /  CHUNK_SIZE_X), (static_cast<float>(z + chunkPosition.z) / CHUNK_SIZE_Z), 2));
                const int waterLevel = 219;
                
                for (int y = 0; y < CHUNK_SIZE_Y; y++) {

                    int worldY = y + chunkPosition.y;

                    if (worldY <= rockHeight) {
                        blocks[x][y][z].type = Block::ROCK;
                    } else {
                        if (worldY == topSoilHeight) {
                            blocks[x][y][z].type = Block::GRASS;
                        } else if (worldY < topSoilHeight) {
                            blocks[x][y][z].type = Block::DIRT;
                        } else if (worldY < waterLevel) {
                            blocks[x][y][z].type = Block::WATER;
                        } else {
                            blocks[x][y][z].type = Block::AIR;
                        }
                    }

                }
            }
        }

    }

    WorldGen(const WorldGen&) = delete;
    WorldGen& operator=(const WorldGen&) = delete;

private:

    PerlinNoise noise;

};
