#pragma once

#include <TP/Scene/SceneNode.hpp>
#include <TP/Scene/VoxelChunk.hpp>
#include <utils/Ray.hpp>

#include <unordered_map>
#include "TP/Camera/Frustrum.hpp"
#include <queue>
#include <set>

#include <TP/Scene/ChunkColumn.hpp>

#include <Defines.hpp>
#include <utils/Math.hpp>

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

class Character;

struct IVec2Hash {
    std::size_t operator()(const glm::ivec2 &vec) const {
        return std::hash<int>()(vec.x) ^ (std::hash<int>()(vec.y) << 1);
    }
};

struct IVec3Hash {
    std::size_t operator()(const glm::ivec3 &vec) const {
        return std::hash<int>()(vec.x) ^ (std::hash<int>()(vec.y) << 1) ^ (std::hash<int>()(vec.z) << 2);
    }
};

class World : public SceneNode {
private:
    std::unordered_map<glm::ivec2, std::shared_ptr<ChunkColumn>, IVec2Hash> chunkColumns;
    std::unordered_map<glm::ivec3, std::shared_ptr<VoxelChunk>, IVec3Hash> chunks;
    std::unordered_map<glm::ivec3, std::shared_ptr<VoxelChunk>, IVec3Hash> visibleChunks;
    Camera *camera;
    float time = 12.0f;
    bool doDaylightCycle = true;

    // Set of dirty columns
    std::set<std::pair<int, int>> getDirtyColumns();

    // Update the sky lights for a given chunk x,z in the chunk column
    void updateSkyLightsInColumn(int x, int z);

    std::thread generationThread;
    std::queue<std::pair<int, int>> generationQueue;
    std::mutex queueMutex;
    std::condition_variable queueCV;
    std::atomic<bool> running = true;

    void generationLoop();
public:
    World();

    ~World();

    void initialGeneration();

    void generateChunkColumn(int x, int z);

    void enqueueChunkGeneration(int x, int z);
    void startGenerationThread();
    void stopGenerationThread();

    // Create an empty chunk at the given CHUNK coordinates and return a pointer to it.
    std::shared_ptr<VoxelChunk> createEmptyChunk(int x, int y, int z);

    // Remove a chunk at the given CHUNK coordinates.
    void removeChunkColumn(int x, int z);

    // Get a chunk column at the given CHUNK coordinates
    std::shared_ptr<ChunkColumn> getChunkColumn(int x, int z);
    
    // Return all chunks in the world
    std::vector<std::shared_ptr<VoxelChunk> > getAllChunks();

    // Get a pointer to the chunk at the given CHUNK coordinates
    std::shared_ptr<VoxelChunk> getChunk(int x, int y, int z);

    // Get a pointer to the chunk at the given WORLD coordinates
    std::shared_ptr<VoxelChunk> getChunkContaining(int x, int y, int z);

    // Get a pointer to the chunk at the given WORLD coordinates (float version)
    std::shared_ptr<VoxelChunk> getChunkContaining(glm::vec3 position);

    /**
     * @brief Get all the chunks that intersect with the given ray and max distance.
     * 
     * @param ray 
     * @param maxDistance 
     * @return std::vector<VoxelChunk*> 
     */
    std::vector<std::shared_ptr<VoxelChunk> > getIntersectedChunks(Ray ray, float maxDistance);

    void updateLoadedChunks();
    void draw(GLuint programID) override;

    /**
     * @brief Remove a block at the given WORLD coordinates. Used by the player.
     * 
     * 
     * @return int Broken block id or -1 if no block was broken
     */
    int playerRemoveBlock(int x, int y, int z, unsigned char gamemode);

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
     * @brief Set a block at the given WORLD coordinates. Does not update the lights (useful for faster generation).
     * 
     * 
     * @return bool True if the block was set, else false
     */
    bool generationSetBloc(int x, int y, int z, int bloc);

    unsigned short getLightLevel(int x, int y, int z);

    /**
     * @brief Get a block at the given WORLD coordinates.
     * 
     * 
     * @return int Block id, -1 if no block was found (do not confuse with AIR which is 0)
     */
    int getBloc(int x, int y, int z);

    void cleanupBuffers() override;

    void updateVisibleChunk(Frustrum &frustum);

    void setCamera(Camera &camera);

    void resolveCollisions(Character &character, World *world);

    void resolveCollisionForBlock(Character &character, glm::vec3 blockPosition);

    void update(float deltaTime);

    inline void setDoDaylightCycle(bool timeRunning) {
        this->doDaylightCycle = timeRunning;
    }

    inline void setTime(float time) {
        this->time = time;
    }

    inline float getTime() {
        return time;
    }

    // Propage la lumière d'un bloc à partir de son emplacement.
    void updateLightFloodfill(int x, int y, int z);

    // Définit la lumière de tous les blocs voisins d'un bloc et propage la lumière.
    void lightFloodFillNeighbors(int x, int y, int z, int lightLevel);

    // Définit le niveau de lumière d'un bloc et propage la lumière.
    void lightFloodfill(int startX, int startY, int startZ, int startLightLevel);

    // Définit le niveau de lumière d'un bloc.
    void setLightLevel(int x, int y, int z, int lightLevel);
};