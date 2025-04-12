#include <TP/Scene/World.hpp>
#include <random>
#include "TP/Camera/Frustrum.hpp"

VoxelChunk *World::createEmptyChunk(int x, int y, int z) {
    chunks.emplace(glm::ivec3(x, y, z), VoxelChunk(CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE));
    VoxelChunk *chunk = &chunks.at(glm::ivec3(x, y, z));
    chunk->translate(glm::vec3(x * CHUNK_SIZE, y * CHUNK_SIZE, z * CHUNK_SIZE));
    return chunk;
}

void World::removeChunk(int x, int y, int z) {
    auto it = chunks.find({x, y, z});
    if (it != chunks.end()) {
        it->second.cleanupBuffers();
        chunks.erase(it);
    }
}

VoxelChunk *World::getChunk(int x, int y, int z) {
    auto it = chunks.find({x, y, z});
    if (it != chunks.end()) {
        return &it->second;
    }
    return nullptr;
}

int World::playerRemoveBlock(int x, int y, int z) {
    VoxelChunk *chunk = getChunkAt(x, y, z);
    if (chunk) {
        return chunk->playerRemoveBlock(x % CHUNK_SIZE, y % CHUNK_SIZE, z % CHUNK_SIZE);
    }
    return -1;
}

int World::removeBlock(int x, int y, int z) {
    VoxelChunk *chunk = getChunkAt(x, y, z);
    if (chunk) {
        return chunk->removeBlock(x % CHUNK_SIZE, y % CHUNK_SIZE, z % CHUNK_SIZE);
    }
    return -1;
}

bool World::setBloc(int x, int y, int z, int bloc) {
    VoxelChunk *chunk = getChunkAt(x, y, z);
    if (chunk) {
        return chunk->setBloc(x % CHUNK_SIZE, y % CHUNK_SIZE, z % CHUNK_SIZE, bloc);
    }
    return false;
}

int World::getBloc(int x, int y, int z) {
    VoxelChunk *chunk = getChunkAt(x, y, z);
    if (chunk) {
        return chunk->getBloc(x % CHUNK_SIZE, y % CHUNK_SIZE, z % CHUNK_SIZE);
    }
    return -1;
}

VoxelChunk *World::getChunkAt(int x, int y, int z) {
    int xx = x / CHUNK_SIZE;
    int yy = y / CHUNK_SIZE;
    int zz = z / CHUNK_SIZE;
    auto it = chunks.find({xx, yy, zz});
    if (it != chunks.end()) {
        return &it->second;
    }
    return nullptr;
}

VoxelChunk *World::getChunkContaining(glm::vec3 position) {
    int x = static_cast<int>(position.x);
    int y = static_cast<int>(position.y);
    int z = static_cast<int>(position.z);
    return getChunkAt(x, y, z);
}

std::vector<VoxelChunk *> World::getIntersectedChunks(Ray ray, float maxDistance) {
    std::vector<VoxelChunk *> intersectedChunks;
    for (auto &[key, chunk]: chunks) {
        if (chunk.intersects(ray, maxDistance)) {
            intersectedChunks.push_back(&chunk);
        }
    }
    return intersectedChunks;
}

void World::generation() {
    FastNoiseLite baseNoise;
    baseNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    baseNoise.SetFrequency(0.1f);

    FastNoiseLite mountainNoise;
    mountainNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    mountainNoise.SetFrequency(0.05f);

    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> treeChance(0, 100); // 5% chance for a tree

    int groundLevel = 4;
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            VoxelChunk *chunk = createEmptyChunk(i, 0, j);
            for (int x = 0; x < CHUNK_SIZE; x++) {
                for (int z = 0; z < CHUNK_SIZE; z++) {
                    int baseHeight = groundLevel + static_cast<int>(baseNoise.GetNoise((float)x + i * CHUNK_SIZE, (float)z + j * CHUNK_SIZE) * 5);

                    chunk->setBloc(x, groundLevel - 4, z, BEDROCK);

                    for (int y = groundLevel - 3; y < baseHeight - 2; y++) {
                        chunk->setBloc(x, y, z, STONE);
                    }

                    chunk->setBloc(x, baseHeight - 2, z, DIRT);
                    chunk->setBloc(x, baseHeight - 1, z, GRASS);

                    int mountainHeight = static_cast<int>(mountainNoise.GetNoise((float)x + i * CHUNK_SIZE, (float)z + j * CHUNK_SIZE) * 10);
                    if (mountainHeight > 0) {
                        for (int y = baseHeight; y < baseHeight + mountainHeight; y++) {
                            chunk->setBloc(x, y, z, STONE);
                        }
                    }

                    // Add trees
                    if (treeChance(rng) < 5 && mountainHeight == 0) { // 5% chance, no trees on mountains
                        int treeHeight = 4 + (treeChance(rng) % 3); // Random tree height between 4 and 6
                        for (int y = 0; y < treeHeight; y++) {
                            chunk->setBloc(x, baseHeight + y, z, LOG_OAK);
                        }
                        for (int dx = -2; dx <= 2; dx++) {
                            for (int dz = -2; dz <= 2; dz++) {
                                if (std::abs(dx) + std::abs(dz) <= 3) { // Simple circular leaf pattern
                                    chunk->setBloc(x + dx, baseHeight + treeHeight, z + dz, LEAVES_OAK);
                                }
                                if (std::abs(dx) + std::abs(dz) <= 2) { // Smaller circular leaf pattern for upper layer
                                    chunk->setBloc(x + dx, baseHeight + treeHeight + 1, z + dz, LEAVES_OAK);
                                }
                                if (std::abs(dx) + std::abs(dz) <= 1) { // Smallest circular leaf pattern for top layer
                                    chunk->setBloc(x + dx, baseHeight + treeHeight + 2, z + dz, LEAVES_OAK);
                                }
                            }
                        }
                    }
                }
            }
            chunk->generateMesh();
        }
    }
}


void World::updateVisibleChunk(Frustrum &frustum) {

    // on parcourt tous les chunks et on les ajoute à la liste des chunks visibles s'ils sont dans le frustum
    for (auto &[key, chunk]: chunks) {
        if (frustum.isBoundingBoxInFrustum(chunk.getWorldPosition(),
                                           chunk.getWorldPosition() + glm::vec3(CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE))) {
            visibleChunks[key] = chunk;
        } else {
            // si le chunk n'est pas visible, on le supprime de la liste des chunks visibles
            auto it = visibleChunks.find(key);
            if (it != visibleChunks.end()) {
                it->second.cleanupBuffers();
                visibleChunks.erase(it);
            }
        }
    }

}
