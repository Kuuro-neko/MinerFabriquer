#pragma once

#include <TP/Scene/SceneNode.hpp>
#include <TP/Scene/VoxelChunk.hpp>
#include <utils/Ray.hpp>

#include <unordered_map>
#include "TP/Camera/Frustrum.hpp"

#define CHUNK_SIZE 16

struct IVec3Hash {
    std::size_t operator()(const glm::ivec3 &vec) const {
        return std::hash<int>()(vec.x) ^ (std::hash<int>()(vec.y) << 1) ^ (std::hash<int>()(vec.z) << 2);
    }
};

class World : public SceneNode {
private:
    std::unordered_map<glm::ivec3, VoxelChunk, IVec3Hash> chunks;
    std::unordered_map<glm::ivec3, VoxelChunk, IVec3Hash> visibleChunks;
    Camera *camera;
public:
    World() : SceneNode(Transform(), new MeshObject(), nullptr) {
        generation();
    }

    ~World() {
    }

    void generation();

    /**
     * @brief Create an empty chunk at the given CHUNK coordinates.
     *  
     * 
     * @return VoxelChunk* pointer to the created chunk
     */
    VoxelChunk *createEmptyChunk(int x, int y, int z);

    /**
     * @brief Remove a chunk at the given CHUNK coordinates.
     * 
     * 
     */
    void removeChunk(int x, int y, int z);

    /**
     * @brief Get a chunk at the given CHUNK coordinates.
     * 
     * 
     * @return VoxelChunk* pointer to the chunk
     */
    VoxelChunk *getChunk(int x, int y, int z);

    /**
     * @brief Get a chunk containing the given WORLD coordinates.
     * 
     * 
     * @return VoxelChunk* pointer to the chunk
     */
    VoxelChunk *getChunkAt(int x, int y, int z);

    /**
     * @brief Get a chunk containing the given WORLD float coordinates.
     * 
     * 
     * @return VoxelChunk* pointer to the chunk
     */
    VoxelChunk *getChunkContaining(glm::vec3 position);

    /**
     * @brief Get all the chunks that intersect with the given ray and max distance.
     * 
     * @param ray 
     * @param maxDistance 
     * @return std::vector<VoxelChunk*> 
     */
    std::vector<VoxelChunk *> getIntersectedChunks(Ray ray, float maxDistance);

    void draw(GLuint programID) override {
        for (auto &[key, chunk]: visibleChunks) {


            //si la distance de rendu est depassee, on ne dessine pas le chunk
            float distance = glm::length(chunk.getWorldPosition() - camera->getPosition());
            // std::cout << "Distance: " << distance << std::endl;
            std::cout << "Camera Position: " << camera->getPosition().x << ", " << camera->getPosition().y << ", "
                      << camera->getPosition().z
                      << std::endl;
            if (distance <= (RENDERER_DISTANCE * CHUNK_SIZE)) {
                chunk.draw(programID);
            }

        }
    }

    /**
     * @brief Remove a block at the given WORLD coordinates. Used by the player.
     * 
     * 
     * @return int Broken block id or -1 if no block was broken
     */
    int playerRemoveBlock(int x, int y, int z);

    /**
     * @brief Remove a block at the given WORLD coordinates. Don't use it for the player directly.
     * 
     * 
     * @return int Broken block id or -1 if no block was broken
     */
    int removeBlock(int x, int y, int z);

    /**
     * @brief Set a block at the given WORLD coordinates.
     * 
     * 
     * @return bool True if the block was set, else false
     */
    bool setBloc(int x, int y, int z, int bloc);

    /**
     * @brief Get a block at the given WORLD coordinates.
     * 
     * 
     * @return int Block id, -1 if no block was found (do not confuse with AIR which is 0)
     */
    int getBloc(int x, int y, int z);

    void cleanupBuffers() override {
        for (auto &[key, chunk]: chunks) {
            chunk.cleanupBuffers();
        }
    }

    void updateVisibleChunk(Frustrum &frustum);

    inline void setCamera(Camera &camera) {
        this->camera = &camera;
    }
};