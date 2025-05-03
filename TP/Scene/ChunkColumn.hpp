#pragma once

#include <TP/Scene/VoxelChunk.hpp>

class ChunkColumn
{
private:
    glm::ivec2 m_chunkCoords;
    // sorted from lowest y to highest y
    std::vector<VoxelChunk*> m_chunks;
    std::vector<std::vector<int>> surfaceHeightmap;

    void sortChunks();
public:
    ChunkColumn(int chunkCoordX, int chunkCoordZ) {
        m_chunkCoords = glm::ivec2(chunkCoordX, chunkCoordZ);
        allocateSurfaceHeightMap();
    }
    ~ChunkColumn() {
        free();
    }


    VoxelChunk* getChunk(int chunkCoordY);
    std::vector<VoxelChunk*> getChunks();
    VoxelChunk* getChunkContainingHeight(int y);
    VoxelChunk* createEmptyChunk(int x, int y, int z);
    
    void updateSkyLights();

    void allocateSurfaceHeightMap();
    std::vector<std::vector<int>> *getSurfaceHeightMap();
    void free();
    bool isDirty();
};