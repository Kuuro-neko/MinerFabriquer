#include <TP/Scene/World.hpp>
#include <random>
#include "WorldGenerator.hpp"

World::World() : SceneNode(Transform(), new MeshObject(), nullptr) {
    generation();
}

World::~World() {
}

VoxelChunk *World::createEmptyChunk(int x, int y, int z) {
    chunks.emplace(glm::ivec3(x, y, z), VoxelChunk(CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE));
    VoxelChunk *chunk = &chunks.at(glm::ivec3(x, y, z));
    chunk->translate(glm::vec3(x * CHUNK_SIZE, y * CHUNK_SIZE, z * CHUNK_SIZE));
    chunk->m_world = this;
    chunk->m_chunkCoords = glm::ivec3(x, y, z);
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
    } else {
        return OUT_OF_BOUNDS_BLOC;
    }
}

VoxelChunk *World::getChunkAt(int x, int y, int z) {
    int xx = (x < 0) ? (x - CHUNK_SIZE + 1) / CHUNK_SIZE : x / CHUNK_SIZE;
    int yy = (y < 0) ? (y - CHUNK_SIZE + 1) / CHUNK_SIZE : y / CHUNK_SIZE;
    int zz = (z < 0) ? (z - CHUNK_SIZE + 1) / CHUNK_SIZE : z / CHUNK_SIZE;
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

void World::draw(GLuint programID) {
    for (auto &[key, chunk]: visibleChunks) {
        //si la distance de rendu est depassee, on ne dessine pas le chunk
        float distance = glm::length(chunk.getWorldPosition() - camera->getPosition());
        if (distance <= (RENDERER_DISTANCE * CHUNK_SIZE)) {
            chunk.draw(programID);
        }
    }
}

void World::generation() {
    WorldGenerator worldGenerator;

    // 2x2 chunks for testing
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            VoxelChunk *chunk = createEmptyChunk(i,0,j);
            worldGenerator.genereteProceduralChunk(chunk, i, j);

        }
    }

    // generate all meshes
    for (auto &[key, chunk]: chunks) {
        chunk.generateMesh();
    }
}

void World::cleanupBuffers() {
    for (auto &[key, chunk]: chunks) {
        chunk.cleanupBuffers();
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

void World::setCamera(Camera &camera) {
    this->camera = &camera;
}

/**
 * @brief Résout les collisions du personnage avec le chunk.
 * @param character Le personnage à vérifier.
 * @param chunk Le chunk actuel dans lequel se trouve le personnage.
 */
void World::resolveCollisions(Character &character, VoxelChunk *chunk) {
    glm::vec3 newPosition = character.getWorldPosition() + character.velocity;

    // Phase 1: Vertical (Y-axis) collision resolution
    if (checkCollision(character, chunk, glm::vec3(0, character.velocity.y, 0))) {
        character.velocity.y = 0; // Stop vertical movement
    } else {
        //character.setPosition(character.getWorldPosition() + glm::vec3(0, character.velocity.y, 0));
        translate(character.getWorldPosition() + glm::vec3(0, character.velocity.y, 0));
    }

    // Phase 2: Horizontal (X-axis) collision resolution
    if (checkCollision(character, chunk, glm::vec3(character.velocity.x, 0, 0))) {
        character.velocity.x = 0; // Stop horizontal movement along X
    } else {
       // character.setPosition(character.getWorldPosition() + glm::vec3(character.velocity.x, 0, 0));
        translate(character.getWorldPosition()+ glm::vec3(character.velocity.x, 0, 0));
    }

    // Phase 3: Horizontal (Z-axis) collision resolution
    if (checkCollision(character, chunk, glm::vec3(0, 0, character.velocity.z))) {
        character.velocity.z = 0; // Stop horizontal movement along Z
    } else {
        //character.setPosition(character.getWorldPosition() + glm::vec3(0, 0, character.velocity.z));
        translate( character.getWorldPosition() + glm::vec3(0, 0, character.velocity.z));
    }
}

/**
 * @brief Vérifie si le personnage entre en collision avec un bloc dans le chunk.
 * @param character Le personnage à vérifier.
 * @param chunk Le chunk actuel dans lequel se trouve le personnage.
 * @param offset : le décalage à appliquer à la position du personnage pour la vérification de collision.
 * @return Vrai s'il y a collision, faux sinon.
 */
bool World::checkCollision(Character &character, VoxelChunk *chunk, glm::vec3 offset) {
    glm::vec3 min = character.getMinBoundingBox() + offset;
    glm::vec3 max = character.getMaxBoundingBox() + offset;
    for (int x = static_cast<int>(min.x); x <= static_cast<int>(max.x); ++x) {
        for (int y = static_cast<int>(min.y); y <= static_cast<int>(max.y); ++y) {
            for (int z = static_cast<int>(min.z); z <= static_cast<int>(max.z); ++z) {
                if (chunk->getBloc(x, y, z) != AIR) { // Check if the block is solid
                    return true;
                }
            }
        }
    }
    return false;
}
