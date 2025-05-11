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

void ChunkColumn::initializeChunksForRead()
{
    m_chunks.clear();
    for (int y = GENERATION_SIZE_Y-1; y >= 0; y--) {
        std::shared_ptr<VoxelChunk> chunk = std::make_shared<VoxelChunk>();
        m_chunks.emplace_back(chunk);
        m_chunks.back()->translate(glm::vec3(m_chunkCoords.x * CHUNK_SIZE, y * CHUNK_SIZE, m_chunkCoords.y * CHUNK_SIZE));
        m_chunks.back()->m_chunkCoords = glm::ivec3(m_chunkCoords.x, y, m_chunkCoords.y);
        m_chunks.back()->m_chunkColumn = shared_from_this();
    }
}


void ChunkColumn::initializeChunks()
{
    free();
    allocateSurfaceHeightMap();

    for (int y = GENERATION_SIZE_Y-1; y >= 0; y--) {
        m_chunks.emplace_back(std::make_shared<VoxelChunk>());
        m_chunks.back()->translate(glm::vec3(m_chunkCoords.x * CHUNK_SIZE, y * CHUNK_SIZE, m_chunkCoords.y * CHUNK_SIZE));
        m_chunks.back()->m_chunkCoords = glm::ivec3(m_chunkCoords.x, y, m_chunkCoords.y);
        m_chunks.back()->m_chunkColumn = shared_from_this();
    }
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
    return m_chunks;
}

std::shared_ptr<VoxelChunk> ChunkColumn::getChunkContainingHeight(int y)
{
    int currentY = m_chunks.size() * CHUNK_SIZE;

    if (y < 0 || y >= currentY) {
        return nullptr;
    }

    // Since chunks are sorted from highest to lowest, we can just iterate from the top, gaining performance
    for (int i = 0; i < m_chunks.size(); ++i) {
        currentY -= CHUNK_SIZE;
        if (y >= currentY) {
            return m_chunks[i];
        }
    }

    return nullptr;
}



void ChunkColumn::addChunk(std::shared_ptr<VoxelChunk> chunk)
{
    m_chunks.push_back(chunk);
    chunk->m_chunkColumn = shared_from_this();
    sortChunks();
}

void ChunkColumn::updateSkyLights()
{
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

bool ChunkColumn::generate()
{
    WorldGenerator& worldGenerator = WorldGenerator::getInstance();

    initializeChunks();

    sortChunks();

    // Generate the world
    for (int y = GENERATION_SIZE_Y-1; y >= 0; y--) {
        std::shared_ptr<VoxelChunk> chunk = getChunk(y);
        worldGenerator.genereteProceduralChunk(chunk, m_chunkCoords.x, y, m_chunkCoords.y);
    }

    for (int y = GENERATION_SIZE_Y-1; y >= 0; y--) {
        std::shared_ptr<VoxelChunk>chunk = getChunk(y);
        worldGenerator.decorateProceduralChunk(chunk, m_chunkCoords.x, y, m_chunkCoords.y);
    }

    // checkForUngeneratedBlocks(westNeighbor);
    // checkForUngeneratedBlocks(eastNeighbor);
    // checkForUngeneratedBlocks(southNeighbor);
    // checkForUngeneratedBlocks(northNeighbor);

    return true;
}

void ChunkColumn::assignWorld(World *world)
{
    for (std::shared_ptr<VoxelChunk> chunk : m_chunks) {
        chunk->m_world = world;
    }
}

int ChunkColumn::getLightLevel(int x, int y, int z)
{
    if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_SIZE * m_chunks.size() || z < 0 || z >= CHUNK_SIZE) {
        return false;
    }
    std::shared_ptr<VoxelChunk> chunk = getChunkContainingHeight(y);
    if (chunk) {
        int localX = x;
        int localY = betterModulo(y, CHUNK_SIZE);
        int localZ = z;
        return chunk->getLightLevel(localX, localY, localZ);
    } else {
        return false;
    }
}

bool ChunkColumn::getBloc(int x, int y, int z)
{
    if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_SIZE * m_chunks.size() || z < 0 || z >= CHUNK_SIZE) {
        return false;
    }
    std::shared_ptr<VoxelChunk> chunk = getChunkContainingHeight(y);
    if (chunk) {
        int localX = x;
        int localY = betterModulo(y, CHUNK_SIZE);
        int localZ = z;
        return chunk->getBloc(localX, localY, localZ);
    } else {
        return false;
    }
}

bool ChunkColumn::setBloc(int x, int y, int z, int bloc)
{
    if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_SIZE * m_chunks.size() || z < 0 || z >= CHUNK_SIZE) {
        return false;
    }
    std::shared_ptr<VoxelChunk> chunk = getChunkContainingHeight(y);
    if (chunk) {
        int localX = x;
        int localY = betterModulo(y, CHUNK_SIZE);
        int localZ = z;
        return chunk->setBloc(localX, localY, localZ, bloc);
    } else {
        return false;
    }
}

bool ChunkColumn::generationSetBloc(int x, int y, int z, int bloc)
{
    if (x < 0 || x >= CHUNK_SIZE || z < 0 || z >= CHUNK_SIZE) {
        return false;
    }
    std::shared_ptr<VoxelChunk> chunk = getChunkContainingHeight(y);
    if (chunk) {
        int localX = x;
        int localY = betterModulo(y, CHUNK_SIZE);
        int localZ = z;
        return chunk->generationSetBloc(localX, localY, localZ, bloc);
    } else {
        return false;
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

bool ChunkColumn::isSkylightDirty()
{
    return skyLightdirty;
}

void ChunkColumn::markSkylightDirty(bool value)
{
    skyLightdirty = value;
}

void ChunkColumn::markChunksAsDirty()
{
    for (std::shared_ptr<VoxelChunk>chunk : m_chunks) {
        chunk->dirty = true;
    }
}
