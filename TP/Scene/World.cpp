#include <TP/Scene/World.hpp>
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
    int groundLevel = 4;
    //chunk #0
    VoxelChunk *chunk = createEmptyChunk(0, 0, 0);
    chunk->setBloc(5, groundLevel + 1, 5, LOG_OAK);
    chunk->setBloc(5, groundLevel + 2, 5, LOG_OAK);
    chunk->setBloc(5, groundLevel + 3, 5, LOG_OAK);
    chunk->setBloc(5, groundLevel + 4, 5, LEAVES_OAK);
    chunk->setBloc(5, groundLevel + 3, 6, LEAVES_OAK);
    chunk->setBloc(6, groundLevel + 3, 5, LEAVES_OAK);
    chunk->setBloc(5, groundLevel + 3, 4, LEAVES_OAK);
    chunk->setBloc(4, groundLevel + 3, 5, LEAVES_OAK);
    for (int i = 0; i < CHUNK_SIZE; i++) {
        for (int j = 0; j < CHUNK_SIZE; j++) {
            chunk->setBloc(i, groundLevel - 4, j, BEDROCK);
            chunk->setBloc(i, groundLevel - 3, j, STONE);
            chunk->setBloc(i, groundLevel - 2, j, DIRT);
            chunk->setBloc(i, groundLevel - 1, j, DIRT);
            chunk->setBloc(i, groundLevel, j, GRASS);
        }
    }
    chunk->setBloc(8, groundLevel + 4, 8, GLOWSTONE);
    chunk->setBloc(8, groundLevel + 4, 9, GLOWSTONE);
    chunk->setBloc(9, groundLevel + 4, 8, GLOWSTONE);
    chunk->setBloc(9, groundLevel + 4, 9, GLOWSTONE);
    chunk->setBloc(8, groundLevel + 5, 8, GLOWSTONE);
    chunk->setBloc(8, groundLevel + 5, 9, GLOWSTONE);
    chunk->setBloc(9, groundLevel + 5, 8, GLOWSTONE);
    chunk->setBloc(9, groundLevel + 5, 9, GLOWSTONE);
    chunk->generateMesh();

    //chunk #1
    chunk = createEmptyChunk(1, 0, 0);
    chunk->setBloc(2, groundLevel + 1, 2, LOG_OAK);
    chunk->setBloc(2, groundLevel + 2, 2, LOG_OAK);
    chunk->setBloc(2, groundLevel + 3, 2, LOG_OAK);
    chunk->setBloc(2, groundLevel + 4, 2, LEAVES_OAK);
    chunk->setBloc(2, groundLevel + 3, 3, LEAVES_OAK);
    chunk->setBloc(3, groundLevel + 3, 2, LEAVES_OAK);
    chunk->setBloc(2, groundLevel + 3, 1, LEAVES_OAK);
    chunk->setBloc(1, groundLevel + 3, 2, LEAVES_OAK);
    for (int i = 0; i < CHUNK_SIZE; i++) {
        for (int j = 0; j < CHUNK_SIZE; j++) {
            chunk->setBloc(i, groundLevel - 4, j, BEDROCK);
            chunk->setBloc(i, groundLevel - 3, j, STONE);
            chunk->setBloc(i, groundLevel - 2, j, DIRT);
            chunk->setBloc(i, groundLevel - 1, j, DIRT);
            chunk->setBloc(i, groundLevel, j, GRASS);
        }
    }
    chunk->setBloc(8, groundLevel + 4, 8, GLOWSTONE);
    chunk->setBloc(8, groundLevel + 4, 9, GLOWSTONE);
    chunk->setBloc(9, groundLevel + 4, 8, GLOWSTONE);
    chunk->setBloc(9, groundLevel + 4, 9, GLOWSTONE);
    chunk->setBloc(8, groundLevel + 5, 8, GLOWSTONE);
    chunk->setBloc(8, groundLevel + 5, 9, GLOWSTONE);
    chunk->setBloc(9, groundLevel + 5, 8, GLOWSTONE);
    chunk->setBloc(9, groundLevel + 5, 9, GLOWSTONE);
    chunk->generateMesh();

    //chunk #2
    chunk = createEmptyChunk(0, 0, 1);
    // Arbre 1
    chunk->setBloc(3, groundLevel + 1, 3, LOG_OAK);
    chunk->setBloc(3, groundLevel + 2, 3, LOG_OAK);
    chunk->setBloc(3, groundLevel + 3, 3, LOG_OAK);
    chunk->setBloc(3, groundLevel + 4, 3, LEAVES_OAK);
    chunk->setBloc(3, groundLevel + 3, 4, LEAVES_OAK);
    chunk->setBloc(4, groundLevel + 3, 3, LEAVES_OAK);
    chunk->setBloc(3, groundLevel + 3, 2, LEAVES_OAK);
    chunk->setBloc(2, groundLevel + 3, 3, LEAVES_OAK);

    // Arbre 2
    chunk->setBloc(7, groundLevel + 1, 7, LOG_OAK);
    chunk->setBloc(7, groundLevel + 2, 7, LOG_OAK);
    chunk->setBloc(7, groundLevel + 3, 7, LOG_OAK);
    chunk->setBloc(7, groundLevel + 4, 7, LEAVES_OAK);
    chunk->setBloc(7, groundLevel + 3, 8, LEAVES_OAK);
    chunk->setBloc(8, groundLevel + 3, 7, LEAVES_OAK);
    chunk->setBloc(7, groundLevel + 3, 6, LEAVES_OAK);
    chunk->setBloc(6, groundLevel + 3, 7, LEAVES_OAK);

    // Arbre 3
    chunk->setBloc(10, groundLevel + 1, 5, LOG_OAK);
    chunk->setBloc(10, groundLevel + 2, 5, LOG_OAK);
    chunk->setBloc(10, groundLevel + 3, 5, LOG_OAK);
    chunk->setBloc(10, groundLevel + 4, 5, LEAVES_OAK);
    chunk->setBloc(10, groundLevel + 3, 6, LEAVES_OAK);
    chunk->setBloc(11, groundLevel + 3, 5, LEAVES_OAK);
    chunk->setBloc(10, groundLevel + 3, 4, LEAVES_OAK);
    chunk->setBloc(9, groundLevel + 3, 5, LEAVES_OAK);

    for (int i = 0; i < CHUNK_SIZE; i++) {
        for (int j = 0; j < CHUNK_SIZE; j++) {
            chunk->setBloc(i, groundLevel - 4, j, BEDROCK);
            chunk->setBloc(i, groundLevel - 3, j, STONE);
            chunk->setBloc(i, groundLevel - 2, j, DIRT);
            chunk->setBloc(i, groundLevel - 1, j, DIRT);
            chunk->setBloc(i, groundLevel, j, GRASS);
        }
    }
    chunk->generateMesh();


    //chunk #3
    chunk = createEmptyChunk(1, 0, 1);
    int peakHeight = groundLevel + 6; // Hauteur maximale de la dune
    for (int i = 0; i < CHUNK_SIZE; i++) {
        for (int j = 0; j < CHUNK_SIZE; j++) {
            int height =
                    groundLevel + std::max(0, peakHeight - std::abs(i - CHUNK_SIZE / 2) - std::abs(j - CHUNK_SIZE / 2));
            for (int k = groundLevel - 4; k <= height; k++) {
                if (k == groundLevel - 4) {
                    chunk->setBloc(i, k, j, BEDROCK);
                } else if (k < height - 2) {
                    chunk->setBloc(i, k, j, STONE);
                } else if (k < height) {
                    chunk->setBloc(i, k, j, DIRT);
                } else {
                    chunk->setBloc(i, k, j, GRASS);
                }
            }
        }
    }
    chunk->generateMesh();
}

void World::updateVisibleChunk(Frustrum &frustum) {

    // on parcourt tous les chunks et on les ajoute à la liste des chunks visibles s'ils sont dans le frustum
    for (auto &[key, chunk]: chunks) {
        if (frustum.isBoundingBoxInFrustum(chunk.getWorldPosition(),
                                           chunk.getWorldPosition() + glm::vec3(CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE))) {
            visibleChunks[key] = chunk;
          //  std::cout << "Chunk " << key.x << "," << key.y << "," << key.z << " is visible" << std::endl;
        } else {
            // si le chunk n'est pas visible, on le supprime de la liste des chunks visibles
            auto it = visibleChunks.find(key);
            if (it != visibleChunks.end()) {
                it->second.cleanupBuffers();
                visibleChunks.erase(it);
                std::cout << "Chunk " << key.x << "," << key.y << "," << key.z << " is not visible" << std::endl;
            }
        }
    }

}
