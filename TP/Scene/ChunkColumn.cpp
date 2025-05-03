#include "ChunkColumn.hpp"

// sort from highest to lowest
void ChunkColumn::sortChunks()
{
    std::sort(m_chunks.begin(), m_chunks.end(), [](const VoxelChunk* a, const VoxelChunk* b) {
        return a->m_chunkCoords.y > b->m_chunkCoords.y;
    });
}

VoxelChunk *ChunkColumn::getChunk(int chunkCoordY)
{
    for (VoxelChunk *chunk : m_chunks) {
        if (chunk->m_chunkCoords.y == chunkCoordY) {
            return chunk;
        }
    }
    return nullptr;
}

std::vector<VoxelChunk*> ChunkColumn::getChunks()
{
    std::vector<VoxelChunk*> chunks;
    for (auto &chunk : m_chunks) {
        chunks.push_back(chunk);
    }
    return chunks;
}

VoxelChunk *ChunkColumn::getChunkContainingHeight(int y)
{
    for (VoxelChunk *chunk : m_chunks) {
        if (chunk->m_chunkCoords.y == y) {
            return chunk;
        }
    }
    return nullptr;
}

void ChunkColumn::addChunk(VoxelChunk* chunk)
{
    m_chunks.push_back(chunk);
    sortChunks();
}

void ChunkColumn::updateSkyLights()
{
    int maxY = m_chunks.back()->m_chunkCoords.y * CHUNK_SIZE + CHUNK_SIZE - 1;

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            updateSkyLights(x, z);
        }
    }
}

void ChunkColumn::updateSkyLights(int x, int z) {
    bool touchedGround = false;
    for (VoxelChunk *chunk: m_chunks) {
        for (int y = CHUNK_SIZE - 1; y >= 0; y--) {
            if (chunk->getBloc(x, y, z) == AIR) {
                chunk->m_lights[x][y][z] = MAX_LIGHT;
            }
            else {
                touchedGround = true;
                this->surfaceHeightmap[x][z] = y + chunk->m_chunkCoords.y * CHUNK_SIZE;
                break;
            }
        }
        if (touchedGround) break;
    }
}

void ChunkColumn::allocateSurfaceHeightMap()
{
    surfaceHeightmap = std::vector<std::vector<int>>(CHUNK_SIZE, std::vector<int>(CHUNK_SIZE, -1));
}

std::vector<std::vector<int>> *ChunkColumn::getSurfaceHeightMap()
{
    if (surfaceHeightmap.empty()) {
        std::cout << "Surface heightmap is empty!" << std::endl;
    }
    return &surfaceHeightmap;
}

void ChunkColumn::free()
{
    m_chunks.clear();
    surfaceHeightmap.clear();
}

bool ChunkColumn::isDirty()
{
    for (VoxelChunk *chunk : m_chunks) {
        if (chunk->dirty) {
            return true;
        }
    }
    return false;
}
