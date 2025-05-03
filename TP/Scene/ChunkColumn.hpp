#pragma once

#include <TP/Scene/VoxelChunk.hpp>

class ChunkColumn
{
private:
    glm::ivec2 m_chunkCoords;
    // sorted from highest y to lowest y
    std::vector<VoxelChunk*> m_chunks;
    // Map of the surface height for each chunk-relative x,z in the chunk column
    std::vector<std::vector<int>> surfaceHeightmap;
public:
    ChunkColumn(int chunkCoordX, int chunkCoordZ) {
        m_chunkCoords = glm::ivec2(chunkCoordX, chunkCoordZ);
        allocateSurfaceHeightMap();
    }
    ~ChunkColumn() {
        free();
    }

    // Return the chunk at the given CHUNK y coordinate
    VoxelChunk* getChunk(int chunkCoordY);
    
    // Return the chunk at the given WORLD y coordinate
    VoxelChunk* getChunkContainingHeight(int y);

    // Return a vector of pointer to all chunks in the column
    std::vector<VoxelChunk*> getChunks();

    // Add a chunk to the column and sort the column's chunks
    void addChunk(VoxelChunk* chunk);
    
    // Update the sky lights for all chunks in the column
    void updateSkyLights();

    // Update the sky lights for a given column x,z in the chunk column
    void updateSkyLights(int x, int z);

    // Get the surfaceHeightmap : a map of the surface height for each chunk-relative x,z in the chunk column
    std::vector<std::vector<int>> *getSurfaceHeightMap();
    
    void allocateSurfaceHeightMap();
    void free();
    bool isDirty();
    void sortChunks();
};