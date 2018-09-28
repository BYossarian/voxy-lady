
#pragma once

#include <tuple>
#include <unordered_map>
#include <math.h>
#include <algorithm>
#include <functional>

#include <glm/glm.hpp>

#include "../libs/camera.h"
#include "../libs/aabb.h"
#include "../libs/multi-threading/thread-pool.h"
#include "../libs/multi-threading/latch.h"
#include "../helpers/timer.h"
#include "./chunk.h"
#include "./block.h"
#include "./world-gen.h"


// NB: As it stands, World assumes that the world is only one chunk high
// (Chunk, however, assumes it can have neighbours in any direction)

class World {

public:

    // chunks will be created and their blocks generated when they are 
    // within CREATE_RADIUS. They will have their meshes generated when 
    // they are within DRAW_RADIUS; and they will be freed when they are 
    // outside OUTER_RADIUS:
    // NB: radius is a bit of a misnomer as currently we're considering 
    // these boundaries to be square:
    static constexpr int DRAW_RADIUS = 16;
    static constexpr int CREATE_RADIUS = DRAW_RADIUS + 1;
    static constexpr int OUTER_RADIUS = CREATE_RADIUS + 1;

    World() {

        int maxNumChunks = std::pow(OUTER_RADIUS + 1, 2);

        drawList.reserve(maxNumChunks);
        chunkProcessingList.reserve(maxNumChunks);

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

        drawList.clear();

        // center of the chunks will be the chunk's position + Chunk::CHUNK_SIZE_X / 2.0 etc. 
        // we can do the + Chunk::CHUNK_SIZE_X bit here to avoid repeating it for each chunk:
        float offsetCameraX = camera.getPosition().x - Chunk::CHUNK_SIZE_X / 2.0;
        float offsetCameraZ = camera.getPosition().z - Chunk::CHUNK_SIZE_Z / 2.0;

        for (auto const& [key, chunk] : chunks) {
            if (camera.canSee(chunk->getAABB()) && chunk->getStatus() == Chunk::Status::COMPLETE) {
                const glm::ivec3& chunkPos = chunk->getPosition();
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
    WorldGen<Chunk::CHUNK_SIZE_X, Chunk::CHUNK_SIZE_Y, Chunk::CHUNK_SIZE_Z> worldGen;
    std::vector<VisibleChunk> drawList;
    std::vector<Chunk*> chunkProcessingList;
    thread_pool threadPool;

    void processChunkList(const std::function<void(Chunk*)> &task) {

        int chunksToProcess = chunkProcessingList.size();

        if (chunksToProcess == 0) { return; }

        int numThreads = threadPool.get_thread_count();
        int chunksPerTask = std::ceil(chunksToProcess / numThreads);
        latch tasks_latch(chunksToProcess);
        int start = 0;
        int end = 0;
        while (chunksToProcess > 0) {
            start = end;
            end = std::min(start + chunksPerTask, static_cast<int>(chunkProcessingList.size()));
            chunksToProcess -= chunksPerTask;
            threadPool.submit([this, start, end, &tasks_latch, &task](){

                for (int i = start; i < end; i++) {

                    task(chunkProcessingList[i]);

                    tasks_latch.count_down();

                }

            });
        }
        tasks_latch.wait();

    }

    void buildChunks(const glm::vec3 &position) {

        int currentI = std::floor(position.x / Chunk::CHUNK_SIZE_X);
        int currentJ = std::floor(position.z / Chunk::CHUNK_SIZE_Z);

        int minI = currentI - CREATE_RADIUS;
        int minJ = currentJ - CREATE_RADIUS;
        int maxI = currentI + 1 + CREATE_RADIUS;
        int maxJ = currentJ + 1 + CREATE_RADIUS;

        Timer timer{};

        chunkProcessingList.clear();

        // create Chunks within required area:
        for (int i = minI; i <= maxI; i++) {
                for (int j = minJ; j <= maxJ; j++) {

                    if (getChunk(i, j) != nullptr) {
                        // chunk already exists
                        continue;
                    }

                    Chunk* chunk = new Chunk();
                    chunks.insert({ std::make_pair(i, j), chunk });
                    chunk->setPosition(glm::ivec3(i * Chunk::CHUNK_SIZE_X, 0, j * Chunk::CHUNK_SIZE_Z ));
                    chunkProcessingList.push_back(chunk);

                }
        }

        processChunkList([this](Chunk* chunk) {
            chunk->generateBlocks(worldGen);
        });

        chunkProcessingList.clear();

        timer.printTime("world gen");
        timer.reset();

        // make sure all chunks within draw radius have a mesh:
        for (auto const& [key, chunk] : chunks) {

            if (chunk->getStatus() == Chunk::Status::COMPLETE) {
                // chunk already has a mesh
                continue;
            }

            if (key.first > minI && key.first < maxI &&
                key.second > minJ && key.second < maxJ) {

                    chunkProcessingList.push_back(chunk);

            }

        }

        processChunkList([this](Chunk* chunk) {

            const glm::ivec3& chunkPos = chunk->getPosition();
            int chunkI = chunkPos.x / Chunk::CHUNK_SIZE_X;
            int chunkJ = chunkPos.z / Chunk::CHUNK_SIZE_Z;

            Chunk::Neighbourhood neighbourhood {
                getChunk(chunkI - 1, chunkJ), // left
                getChunk(chunkI + 1, chunkJ), // right
                nullptr, // top
                nullptr, // bottom
                getChunk(chunkI, chunkJ + 1), // front
                getChunk(chunkI, chunkJ - 1)  // back
            };
            chunk->generateMesh(neighbourhood);

        });

        // pushing meshes to the GPU seems to need doing on the main thread:
        // TODO: look into multi-threading and OpenGL a bit more!!
        int chunksToProcess = chunkProcessingList.size();
        for (int i = 0; i < chunksToProcess; i++) {
            chunkProcessingList[i]->syncMesh();
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
