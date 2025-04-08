#pragma once

#include <TP/Scene/SceneNode.hpp>
#include <TP/Scene/VoxelChunk.hpp>
#include <utils/Ray.hpp>

#include <unordered_map>

#define CHUNK_SIZE 16

struct IVec3Hash {
    std::size_t operator()(const glm::ivec3& vec) const {
        return std::hash<int>()(vec.x) ^ (std::hash<int>()(vec.y) << 1) ^ (std::hash<int>()(vec.z) << 2);
    }
};

class World : public SceneNode
{
private:
    std::unordered_map<glm::ivec3, VoxelChunk, IVec3Hash> chunks;
public:
    World() : SceneNode(Transform(), new MeshObject(), nullptr)
    {
        generation();
    }
    ~World() {
    }
    void generation();
    void addChunk(int x, int y, int z, VoxelChunk chunk) {
        chunks.emplace(std::make_tuple(x, y, z), chunk);
        chunk.translate(glm::vec3(x * chunk.m_sizeX, y * chunk.m_sizeY, z * chunk.m_sizeZ));
    }
    void removeChunk(int x, int y, int z) {
        auto it = chunks.find({x, y, z});
        if (it != chunks.end()) {
            it->second.cleanupBuffers();
            chunks.erase(it);
        }
    }
    VoxelChunk* getChunk(int x, int y, int z) {
        auto it = chunks.find({x, y, z});
        if (it != chunks.end()) {
            return &it->second;
        }
        return nullptr;
    }

    void draw(GLuint programID) override {
        for (auto& [key, chunk] : chunks) {
            chunk.draw(programID);
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
            chunk.cleanupBuffers();
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
            return &it->second;
        }
        return nullptr;
    }

    VoxelChunk* getChunkContaining(glm::vec3 position) {
        int x = static_cast<int>(position.x);
        int y = static_cast<int>(position.y);
        int z = static_cast<int>(position.z);
        return getChunkAt(x, y, z);
    }

    std::vector<VoxelChunk*> getIntersectedChunks(Ray ray, float maxDistance) {
        std::vector<VoxelChunk*> intersectedChunks;
        for (auto& [key, chunk] : chunks) {
            if (chunk.intersects(ray, maxDistance)) {
                intersectedChunks.push_back(&chunk);
            }
        }
        return intersectedChunks;
    }
};