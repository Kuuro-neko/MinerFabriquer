#include "ChunkColumn.hpp"
#include <TP/Scene/WorldGenerator.hpp>
#include <TP/Scene/World.hpp>
#include <TP/Scene/VoxelChunk.hpp>

// sort from highest to lowest
void ChunkColumn::sortChunks()
{
    std::sort(m_chunks.begin(), m_chunks.end(), [](const std::shared_ptr<VoxelChunk> a, const std::shared_ptr<VoxelChunk> b) {
        return a->m_chunkCoords.y > b->m_chunkCoords.y;
    });
}

std::shared_ptr<VoxelChunk> ChunkColumn::getChunk(int chunkCoordY)
{
    for (std::shared_ptr<VoxelChunk> chunk : m_chunks) {
        if (chunk->m_chunkCoords.y == chunkCoordY) {
            return chunk;
        }
    }
    return nullptr;
}

std::vector<std::shared_ptr<VoxelChunk>> ChunkColumn::getChunks()
{
    std::vector<std::shared_ptr<VoxelChunk>> chunks;
    for (auto &chunk : m_chunks) {
        chunks.push_back(chunk);
    }
    return chunks;
}

std::shared_ptr<VoxelChunk> ChunkColumn::getChunkContainingHeight(int y)
{
    for (std::shared_ptr<VoxelChunk> chunk : m_chunks) {
        if (chunk->m_chunkCoords.y == y) {
            return chunk;
        }
    }
    return nullptr;
}

void ChunkColumn::addChunk(std::shared_ptr<VoxelChunk> chunk)
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
    for (std::shared_ptr<VoxelChunk> chunk: m_chunks) {
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

void ChunkColumn::checkForUngeneratedBlocks(std::shared_ptr<ChunkColumn> neighbor) {
    if (!neighbor) return;
    std::vector<std::shared_ptr<VoxelChunk>> nChunks = neighbor->getChunks();
    for (std::shared_ptr<VoxelChunk> chunk : nChunks) {
        std::vector<UngeneratedBlock> unGeneratedBlocks = chunk->m_unGeneratedBlocks;
        for (auto &block : unGeneratedBlocks) {
            if (block.chunkX == m_chunkCoords.x && block.chunkZ == m_chunkCoords.y) {
                chunk->setBloc(block.x, block.y, block.z, block.bloc);
            }
        }
    }
}

void ChunkColumn::generate(World &world, std::shared_ptr<ChunkColumn> westNeighbor, std::shared_ptr<ChunkColumn> eastNeighbor, std::shared_ptr<ChunkColumn> southNeighbor, std::shared_ptr<ChunkColumn> northNeighbor)
{
    WorldGenerator worldGenerator;
    // Generate the world
    for (int y = GENERATION_SIZE_Y-1; y >= 0; --y) {
        std::shared_ptr<VoxelChunk>chunk = getChunk(y);
        worldGenerator.genereteProceduralChunk(world, chunk, m_chunkCoords.x, y, m_chunkCoords.y);
    }

    for (int y = GENERATION_SIZE_Y-1; y >= 0; --y) {
        std::shared_ptr<VoxelChunk>chunk = getChunk(y);
        worldGenerator.decorateProceduralChunk(world, chunk, m_chunkCoords.x, y, m_chunkCoords.y);
    }

    // checkForUngeneratedBlocks(westNeighbor);
    // checkForUngeneratedBlocks(eastNeighbor);
    // checkForUngeneratedBlocks(southNeighbor);
    // checkForUngeneratedBlocks(northNeighbor);
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
    for (std::shared_ptr<VoxelChunk>chunk : m_chunks) {
        if (chunk->dirty) {
            return true;
        }
    }
    return false;
}

void ChunkColumn::markAsDirty()
{
    for (std::shared_ptr<VoxelChunk>chunk : m_chunks) {
        chunk->dirty = true;
    }
}
