#include "ChunkColumn.hpp"

// sort from lowest y to highest y
void ChunkColumn::sortChunks()
{
    std::sort(m_chunks.begin(), m_chunks.end(), [](const VoxelChunk &a, const VoxelChunk &b) {
        return a.m_chunkCoords.y < b.m_chunkCoords.y;
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

VoxelChunk* ChunkColumn::createEmptyChunk(int x, int y, int z)
{
    m_chunks.emplace_back(VoxelChunk(CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE));
    VoxelChunk *chunk = m_chunks.back();
    chunk->m_chunkCoords = glm::ivec3(x, y, z);
    chunk->translate(glm::vec3(x * CHUNK_SIZE, y * CHUNK_SIZE, z * CHUNK_SIZE));
}

void ChunkColumn::updateSkyLights()
{
    /*
    
    std::vector<VoxelChunk *> chunksInColumn;
    for (auto &[key, chunk]: chunks) {
        if (chunk.m_chunkCoords.x == x && chunk.m_chunkCoords.z == z) {
            chunksInColumn.push_back(&chunk);
        }
    }
    if (chunksInColumn.empty()) {
        return;
    }
    // order them by lhighest y to lowest y
    std::sort(chunksInColumn.begin(), chunksInColumn.end(), [](VoxelChunk *a, VoxelChunk *b) {
        return a->m_chunkCoords.y > b->m_chunkCoords.y;
    });

    int maxY = chunksInColumn[0]->m_chunkCoords.y * CHUNK_SIZE + CHUNK_SIZE - 1;

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            bool touchedGround = false;
            for (VoxelChunk *chunk: chunksInColumn) {
                for (int y = CHUNK_SIZE - 1; y >= 0; y--) {
                    if (chunk->getBloc(x, y, z) == AIR) {
                        chunk->m_lights[x][y][z] = MAX_LIGHT;
                    }
                    else {
                        touchedGround = true;
                        this->lightFloodfill(chunk->m_chunkCoords.x * CHUNK_SIZE + x, chunk->m_chunkCoords.y * CHUNK_SIZE + y + 1, chunk->m_chunkCoords.z * CHUNK_SIZE + z, MAX_LIGHT);
                        break;
                    }
                }
                if (touchedGround) break;
            }
        }
    }
        */

    sortChunks();
    int maxY = m_chunks.back()->m_chunkCoords.y * CHUNK_SIZE + CHUNK_SIZE - 1;

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            bool touchedGround = false;
            for (VoxelChunk *chunk: m_chunks) {
                for (int y = CHUNK_SIZE - 1; y >= 0; y--) {
                    if (chunk->getBloc(x, y, z) == AIR) {
                        chunk->m_lights[x][y][z] = MAX_LIGHT;
                    }
                    else {
                        touchedGround = true;
                        this->surfaceHeightmap[x][z] = y;
                        break;
                    }
                }
                if (touchedGround) break;
            }
        }
    }
}

void ChunkColumn::allocateSurfaceHeightMap()
{
    surfaceHeightmap = std::vector<std::vector<int>>(CHUNK_SIZE, std::vector<int>(CHUNK_SIZE, -1));
}

std::vector<std::vector<int>> *ChunkColumn::getSurfaceHeightMap()
{
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
