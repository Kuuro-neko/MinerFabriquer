#pragma once

#include <TP/Scene/VoxelChunk.hpp>

class ChunkColumn : public std::enable_shared_from_this<ChunkColumn>
{
private:
    glm::ivec2 m_chunkCoords;
    // sorted from highest y to lowest y
    std::vector<std::shared_ptr<VoxelChunk>> m_chunks;
    // Map of the surface height for each chunk-relative x,z in the chunk column
    std::vector<std::vector<int>> surfaceHeightmap;
    bool skyLightdirty = true;
public:
    ChunkColumn() = default; // Constructeur par défaut
    ChunkColumn(int chunkCoordX, int chunkCoordZ) {
        m_chunkCoords = glm::ivec2(chunkCoordX, chunkCoordZ);
        allocateSurfaceHeightMap();
    }
    ~ChunkColumn() {
        free();
    }

    void initializeChunks();
    void initializeChunksForRead();

    void setHeightmapValue(int x, int z, int value) {
        surfaceHeightmap[x][z] = value;
    }

    // Return the chunk at the given CHUNK y coordinate
    std::shared_ptr<VoxelChunk> getChunk(int chunkCoordY);
    
    // Return the chunk at the given WORLD y coordinate
    std::shared_ptr<VoxelChunk> getChunkContainingHeight(int y);

    // Return a vector of pointer to all chunks in the column
    std::vector<std::shared_ptr<VoxelChunk>> getChunks();

    // Add a chunk to the column and sort the column's chunks
    void addChunk(std::shared_ptr<VoxelChunk> chunk);
    
    // Update the sky lights for all chunks in the column
    void updateSkyLights();

    // Update the sky lights for a given column x,z in the chunk column
    void updateSkyLights(int x, int z);

    // Get the surfaceHeightmap : a map of the surface height for each chunk-relative x,z in the chunk column
    std::vector<std::vector<int>> *getSurfaceHeightMap();

    inline glm::ivec2 getChunkCoords() { return m_chunkCoords; }


    void checkForUngeneratedBlocks(std::shared_ptr<ChunkColumn> neighbor);
    bool generate();
    void assignWorld(World *world);
    
    int getLightLevel(int x, int y, int z);
    bool getBloc(int x, int y, int z);
    bool setBloc(int x, int y, int z, int bloc);
    bool generationSetBloc(int x, int y, int z, int bloc);
    
    void allocateSurfaceHeightMap();
    void free();
    bool isSkylightDirty();
    void markSkylightDirty(bool value);
    void markChunksAsDirty();
    void sortChunks();

    // Move constructor and assignment operator
    ChunkColumn(ChunkColumn&& other) noexcept : m_chunkCoords(other.m_chunkCoords), m_chunks(std::move(other.m_chunks)), surfaceHeightmap(std::move(other.surfaceHeightmap)), skyLightdirty(other.skyLightdirty) {
        other.m_chunkCoords = glm::ivec2(0, 0);
        other.m_chunks.clear();
        other.surfaceHeightmap.clear();
    }

    ChunkColumn& operator=(ChunkColumn&& other) noexcept {
        if (this != &other) {
            m_chunkCoords = other.m_chunkCoords;
            m_chunks = std::move(other.m_chunks);
            surfaceHeightmap = std::move(other.surfaceHeightmap);
            skyLightdirty = other.skyLightdirty;

            other.m_chunkCoords = glm::ivec2(0, 0);
            other.m_chunks.clear();
            other.surfaceHeightmap.clear();
        }
        return *this;
    }

    // Copy constructor and assignment operator are deleted to prevent copying
    ChunkColumn(const ChunkColumn&) = delete;
    ChunkColumn& operator=(const ChunkColumn&) = delete;
};