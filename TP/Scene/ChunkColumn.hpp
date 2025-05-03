#pragma once

#include <TP/Scene/VoxelChunk.hpp>

class ChunkColumn
{
private:
    glm::ivec2 m_chunkCoords;
    // sorted from lowest y to highest y
    std::vector<VoxelChunk*> m_chunks;
    std::vector<std::vector<int>> surfaceHeightmap;
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
    void addChunk(VoxelChunk* chunk);
    
    void updateSkyLights();
    void updateSkyLights(int x, int z);

    void allocateSurfaceHeightMap();
    std::vector<std::vector<int>> *getSurfaceHeightMap();
    void free();
    bool isDirty();
    void sortChunks();
};