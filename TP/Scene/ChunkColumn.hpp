#pragma once

#include <TP/Scene/VoxelChunk.hpp>

class ChunkColumn : public VoxelChunk
{
private:
    glm::ivec2 m_chunkCoords;
    std::vector<VoxelChunk*> m_chunks;
    std::vector<std::vector<int>> skyLightHeightMap;
public:
    ChunkColumn(int chunkCoordX, int chunkCoordZ);
    ChunkColumn();
    ~ChunkColumn();

    void addChunk(VoxelChunk* chunk);

    VoxelChunk* getChunk(int chunkCoordY);
    VoxelChunk* getChunkContainingHeight(int y);

    void allocateSkyLightHeightMap();
};