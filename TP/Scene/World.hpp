#pragma once

#include <TP/Scene/SceneNode.hpp>
#include <TP/Scene/VoxelChunk.hpp>

#include <unordered_map>

#define CHUNK_SIZE 16

struct TupleHash {
    template <typename T>
    std::size_t operator()(const T& tuple) const {
        auto hash1 = std::hash<int>()(std::get<0>(tuple));
        auto hash2 = std::hash<int>()(std::get<1>(tuple));
        auto hash3 = std::hash<int>()(std::get<2>(tuple));
        return hash1 ^ (hash2 << 1) ^ (hash3 << 2); // Combine the hashes
    }
};


class World : public SceneNode
{
private:
    std::unordered_map<std::tuple<int, int, int>, VoxelChunk*, TupleHash> chunks;
public:
    World() : SceneNode(Transform(), new MeshObject(), nullptr)
    {
        int groundLevel = 4;
        VoxelChunk chunk = VoxelChunk(CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE);
        chunk.setBloc(2, groundLevel+1, 2, LOG_OAK);
        chunk.setBloc(2, groundLevel+2, 2, LOG_OAK);
        chunk.setBloc(2, groundLevel+3, 2, LOG_OAK);
        chunk.setBloc(2, groundLevel+4, 2, LEAVES_OAK);
        chunk.setBloc(2, groundLevel+3, 3, LEAVES_OAK);
        chunk.setBloc(3, groundLevel+3, 2, LEAVES_OAK);
        chunk.setBloc(2, groundLevel+3, 1, LEAVES_OAK);
        chunk.setBloc(1, groundLevel+3, 2, LEAVES_OAK);
        for (int i = 0; i < CHUNK_SIZE; i++) {
            for (int j = 0; j < CHUNK_SIZE; j++) {
                chunk.setBloc(i, groundLevel-4, j, BEDROCK);
                chunk.setBloc(i, groundLevel-3, j, STONE);
                chunk.setBloc(i, groundLevel-2, j, DIRT);
                chunk.setBloc(i, groundLevel-1, j, DIRT);
                chunk.setBloc(i, groundLevel, j, GRASS);
            }
        }
        chunk.setBloc(8, groundLevel+4, 8, GLOWSTONE);
        chunk.setBloc(8, groundLevel+4, 9, GLOWSTONE);
        chunk.setBloc(9, groundLevel+4, 8, GLOWSTONE);
        chunk.setBloc(9, groundLevel+4, 9, GLOWSTONE);
        chunk.setBloc(8, groundLevel+5, 8, GLOWSTONE);
        chunk.setBloc(8, groundLevel+5, 9, GLOWSTONE);
        chunk.setBloc(9, groundLevel+5, 8, GLOWSTONE);
        chunk.setBloc(9, groundLevel+5, 9, GLOWSTONE);
        chunk.generateMesh();
    
        VoxelChunk chunk2 = VoxelChunk(CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE);
        chunk2.setBloc(2, groundLevel+1, 2, LOG_OAK);
        chunk2.setBloc(2, groundLevel+2, 2, LOG_OAK);
        chunk2.setBloc(2, groundLevel+3, 2, LOG_OAK);
        chunk2.setBloc(2, groundLevel+4, 2, LEAVES_OAK);
        chunk2.setBloc(2, groundLevel+3, 3, LEAVES_OAK);
        chunk2.setBloc(3, groundLevel+3, 2, LEAVES_OAK);
        chunk2.setBloc(2, groundLevel+3, 1, LEAVES_OAK);
        chunk2.setBloc(1, groundLevel+3, 2, LEAVES_OAK);
        for (int i = 0; i < CHUNK_SIZE; i++) {
            for (int j = 0; j < CHUNK_SIZE; j++) {
                chunk2.setBloc(i, groundLevel-4, j, BEDROCK);
                chunk2.setBloc(i, groundLevel-3, j, STONE);
                chunk2.setBloc(i, groundLevel-2, j, DIRT);
                chunk2.setBloc(i, groundLevel-1, j, DIRT);
                chunk2.setBloc(i, groundLevel, j, GRASS);
            }
        }
        chunk2.setBloc(8, groundLevel+4, 8, GLOWSTONE);
        chunk2.setBloc(8, groundLevel+4, 9, GLOWSTONE);
        chunk2.setBloc(9, groundLevel+4, 8, GLOWSTONE);
        chunk2.setBloc(9, groundLevel+4, 9, GLOWSTONE);
        chunk2.setBloc(8, groundLevel+5, 8, GLOWSTONE);
        chunk2.setBloc(8, groundLevel+5, 9, GLOWSTONE);
        chunk2.setBloc(9, groundLevel+5, 8, GLOWSTONE);
        chunk2.setBloc(9, groundLevel+5, 9, GLOWSTONE);
        chunk2.generateMesh();

        addChunk(0, 0, 0, &chunk);
        addChunk(1, 0, 0, &chunk2);
    }
    ~World() {
        for (auto& [key, chunk] : chunks) {
            delete chunk;
        }
    }
    void addChunk(int x, int y, int z, VoxelChunk* chunk) {
        chunks[{x, y, z}] = chunk;
        chunk->translate(glm::vec3(x * chunk->m_sizeX, y * chunk->m_sizeY, z * chunk->m_sizeZ));
    }
    void removeChunk(int x, int y, int z) {
        auto it = chunks.find({x, y, z});
        if (it != chunks.end()) {
            delete it->second;
            chunks.erase(it);
        }
    }
    VoxelChunk* getChunk(int x, int y, int z) {
        auto it = chunks.find({x, y, z});
        if (it != chunks.end()) {
            return it->second;
        }
        return nullptr;
    }

    void draw(GLuint programID) override {
        for (auto& [key, chunk] : chunks) {
            chunk->draw(programID);
        }
    }

    int playerRemoveBlock(int x, int y, int z) {
        VoxelChunk* chunk = getChunkAt(x, y, z);
        if (chunk) {
            return chunk->playerRemoveBlock(x % CHUNK_SIZE, y % CHUNK_SIZE, z % CHUNK_SIZE);
        }
        return -1;
    }

    int removeBlock(int x, int y, int z) {
        VoxelChunk* chunk = getChunkAt(x, y, z);
        if (chunk) {
            return chunk->removeBlock(x % CHUNK_SIZE, y % CHUNK_SIZE, z % CHUNK_SIZE);
        }
        return -1;
    }

    bool setBloc(int x, int y, int z, int bloc) {
        VoxelChunk* chunk = getChunkAt(x, y, z);
        if (chunk) {
            return chunk->setBloc(x % CHUNK_SIZE, y % CHUNK_SIZE, z % CHUNK_SIZE, bloc);
        }
        return false;
    }

    void cleanupBuffers() override {
        for (auto& [key, chunk] : chunks) {
            chunk->cleanupBuffers();
        }
    }

    int getBloc(int x, int y, int z) {
        VoxelChunk* chunk = getChunkAt(x, y, z);
        if (chunk) {
            return chunk->getBloc(x % CHUNK_SIZE, y % CHUNK_SIZE, z % CHUNK_SIZE);
        }
        return -1;
    }

    VoxelChunk* getChunkAt(int x, int y, int z) {
        int xx = x / CHUNK_SIZE;
        int yy = y / CHUNK_SIZE;
        int zz = z / CHUNK_SIZE;
        auto it = chunks.find({xx, yy, zz});
        if (it != chunks.end()) {
            return it->second;
        }
        return nullptr;
    }

    VoxelChunk* getChunkContaining(glm::vec3 position) {
        int x = static_cast<int>(position.x);
        int y = static_cast<int>(position.y);
        int z = static_cast<int>(position.z);
        return getChunkAt(x, y, z);
    }
};