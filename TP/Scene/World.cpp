#include <TP/Scene/World.hpp>
#include <random>
#include "WorldGenerator.hpp"
#include "TP/Character/Character.hpp"

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
    for (int i = 0; i < 2; i++) { // POur tester collision on met que 1 chunk
        for (int j = 0; j < 2; j++) {
            VoxelChunk *chunk = createEmptyChunk(i, 0, j);
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
    // Récupération de la bounding box du personnage
    glm::vec3 minBB = character.getMinBoundingBox();
    glm::vec3 maxBB = character.getMaxBoundingBox();


    // Broad Phase -> on ne parcourt que les blocs proches de la bounding box du personnage

    // Narrow Phase -> on ne parcourt que les blocs solides

    // Parcours des blocs proches de la bounding box du personnage
    for (int x = static_cast<int>(minBB.x); x <= static_cast<int>(maxBB.x); ++x) {
        for (int y = static_cast<int>(minBB.y); y <= static_cast<int>(maxBB.y); ++y) {
            for (int z = static_cast<int>(minBB.z); z <= static_cast<int>(maxBB.z); ++z) {
                if (chunk->getBloc(x, y, z) != AIR) { // Bloc solide détecté
                    std::cout << "Type de bloc collisioné : " << chunk->getBloc(x, y, z) << std::endl;
                    // Resolution de la collision pour le bloc solide détecté
                    resolveCollisionForBlock(character, glm::vec3(x, y, z));
                }
            }
        }
    }
}

/**
 * @brief Résout la collision entre le personnage et un bloc donné.
 * @param character Le personnage.
 * @param blockPosition La position du bloc solide intersecté
 */
void World::resolveCollisionForBlock(Character &character, glm::vec3 blockPosition) {
    // STRAT GILLES

    // dans le cas où le personnage va aller vers un coin de voxel, actuellemet, on le déplace d'une valeur fixe, ducoup on a un effet de pousse non oushaité
    // ducoup la stratégie est de faire une projection sur les axes x et z du personnage pour calculer la bonne valeur de repoussement
    // on va donc faire la projection de son front avec l'axe x , pareil pour z




    // Bounding box du personnage
    glm::vec3 minBB = character.getMinBoundingBox();
    glm::vec3 maxBB = character.getMaxBoundingBox();

    // Position minimal du bloc intersecté
    glm::vec3 blockMin = blockPosition;
    // Position maximal du bloc intersecté (on rajoute 1 car le bloc est de taille 1x1x1)
    glm::vec3 blockMax = blockPosition + glm::vec3(1.0f);

    // si on a une collision entre le personnage et le bloc
    if (maxBB.x > blockMin.x && minBB.x < blockMax.x &&
        maxBB.y > blockMin.y && minBB.y < blockMax.y &&
        maxBB.z > blockMin.z && minBB.z < blockMax.z) {

        // Calcul de la profondeur de la collision sur chaque axe
        float overlapX = std::min(maxBB.x - blockMin.x, blockMax.x - minBB.x);
        float overlapY = std::min(maxBB.y - blockMin.y, blockMax.y - minBB.y);
        float overlapZ = std::min(maxBB.z - blockMin.z, blockMax.z - minBB.z);

        // Déterminer l'axe avec la plus petite profondeur de collision
        if (overlapX < overlapY && overlapX < overlapZ) { // Axe X
            float correction = (character.getWorldPosition().x < blockPosition.x) ? -overlapX : overlapX;
            character.translate(glm::vec3(correction, 0.f, 0.f));
        } else if (overlapY < overlapX && overlapY < overlapZ) { // Axe Y
            float correction = (character.getWorldPosition().y < blockPosition.y) ? -overlapY : overlapY;
            character.translate(glm::vec3(0.f, correction, 0.f));
        } else { // Axe Z
            float correction = (character.getWorldPosition().z < blockPosition.z) ? -overlapZ : overlapZ;
            character.translate(glm::vec3(0.f, 0.f, correction));
        }
    }
}