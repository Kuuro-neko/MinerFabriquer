#include <TP/Scene/World.hpp>
#include <random>
#include "WorldGenerator.hpp"
#include "TP/Character/Character.hpp"
#include <algorithm>

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

void World::resolveCollisionForBlock(Character &character, glm::vec3 blockPosition) {
    // Bounding box du personnage
    glm::vec3 minBB = character.getMinBoundingBox();
    glm::vec3 maxBB = character.getMaxBoundingBox();

    // Position minimale et maximale du bloc intersecté
    glm::vec3 blockMin = blockPosition;
    glm::vec3 blockMax = blockPosition + glm::vec3(1.0f);

    // Vérification si la bounding box du personnage intersecte celle du bloc
    if (maxBB.x > blockMin.x && minBB.x < blockMax.x &&
        maxBB.y > blockMin.y && minBB.y < blockMax.y &&
        maxBB.z > blockMin.z && minBB.z < blockMax.z) {

        // Calcul des overlaps sur chaque axe
        float overlapX = std::min(maxBB.x - blockMin.x, blockMax.x - minBB.x);
        float overlapY = std::min(maxBB.y - blockMin.y, blockMax.y - minBB.y);
        float overlapZ = std::min(maxBB.z - blockMin.z, blockMax.z - minBB.z);

        // Détermination de l'axe avec la plus petite profondeur de collision
        if (overlapX < overlapY && overlapX < overlapZ) { // Axe X
            if (character.vecteurDirection.x > 0 && character.getWorldPosition().x < blockPosition.x) {
                character.vecteurDirection.x = 0; // Bloque le mouvement vers la droite
            } else if (character.vecteurDirection.x < 0 && character.getWorldPosition().x > blockPosition.x) {
                character.vecteurDirection.x = 0; // Bloque le mouvement vers la gauche
            }
        } else if (overlapY < overlapX && overlapY < overlapZ) { // Axe Y
            if (character.vecteurDirection.y > 0 && character.getWorldPosition().y < blockPosition.y) {
                character.vecteurDirection.y = 0; // Bloque le mouvement vers le haut
            } else if (character.vecteurDirection.y < 0 && character.getWorldPosition().y > blockPosition.y) {
                character.vecteurDirection.y = 0; // Bloque le mouvement vers le bas
            }
        } else { // Axe Z
            if (character.vecteurDirection.z > 0 && character.getWorldPosition().z < blockPosition.z) {
                character.vecteurDirection.z = 0; // Bloque le mouvement vers l'avant
            } else if (character.vecteurDirection.z < 0 && character.getWorldPosition().z > blockPosition.z) {
                character.vecteurDirection.z = 0; // Bloque le mouvement vers l'arrière
            }
        }
    }
}

void World::resolveCollisions(Character &character, VoxelChunk *chunk) {
    // Récupération de la position du personnage
    glm::vec3 characterPosition = character.getWorldPosition();
    glm::vec3 ijkActual = chunk->getChunkCoords();

    std::cout << " ijkActual " << ijkActual.x
              << ijkActual.y << ijkActual.z;

//    VoxelChunk *chunkGauche = getChunkAt(xLeft, ijkActual.y, ijkActual.z);
//    VoxelChunk *chunkDroite = getChunkAt(xRight, ijkActual.y, ijkActual.z);
//    VoxelChunk *chunkHaut = getChunkAt(ijkActual.x, ijkActual.y, zTop);
//    VoxelChunk *chunkBas = getChunkAt(ijkActual.x, ijkActual.y, zBottom);
//
//    std::cout << "chunk gauche : " << (chunkGauche ? chunkGauche->getChunkCoords().x : -1) << ", "
//              << (chunkGauche ? chunkGauche->getChunkCoords().y : -1) << ", "
//              << (chunkGauche ? chunkGauche->getChunkCoords().z : -1) << std::endl;
//    std::cout << "chunk droite : " << (chunkDroite ? chunkDroite->getChunkCoords().x : -1) << ", "
//              << (chunkDroite ? chunkDroite->getChunkCoords().y : -1) << ", "
//              << (chunkDroite ? chunkDroite->getChunkCoords().z : -1) << std::endl;
//    std::cout << "chunk haut : " << (chunkHaut ? chunkHaut->getChunkCoords().x : -1) << ", "
//              << (chunkHaut ? chunkHaut->getChunkCoords().y : -1) << ", "
//              << (chunkHaut ? chunkHaut->getChunkCoords().z : -1) << std::endl;
//    std::cout << "chunk bas : " << (chunkBas ? chunkBas->getChunkCoords().x : -1) << ", "
//              << (chunkBas ? chunkBas->getChunkCoords().y : -1) << ", "
//              << (chunkBas ? chunkBas->getChunkCoords().z : -1) << std::endl;
//
//    // Calcul des distances uniquement si les chunks existent
//    float distanceGauche = chunkGauche ? glm::length(characterPosition - chunkGauche->getWorldPosition()) : FLT_MAX;
//    float distanceDroite = chunkDroite ? glm::length(characterPosition - chunkDroite->getWorldPosition()) : FLT_MAX;
//    float distanceHaut = chunkHaut ? glm::length(characterPosition - chunkHaut->getWorldPosition()) : FLT_MAX;
//    float distanceBas = chunkBas ? glm::length(characterPosition - chunkBas->getWorldPosition()) : FLT_MAX;
//
//// Récupération de la distance minimale
//    float minDistance = std::min({distanceGauche, distanceDroite, distanceHaut, distanceBas});
//    VoxelChunk *chunkProche = chunk; // Initialisation avec le chunk actuel
//    if (minDistance == distanceGauche && chunkGauche) {
//        chunkProche = chunkGauche;
//    } else if (minDistance == distanceDroite && chunkDroite) {
//        chunkProche = chunkDroite;
//    } else if (minDistance == distanceHaut && chunkHaut) {
//        chunkProche = chunkHaut;
//    } else if (minDistance == distanceBas && chunkBas) {
//        chunkProche = chunkBas;
//    }
//
//    //affichage des coordonnées du chunk proche
//    std::cout << "Chunk proche : " << chunkProche->getChunkCoords().x << ", " << chunkProche->getChunkCoords().y
//              << ", " << chunkProche->getChunkCoords().z << std::endl;

// Récupération de la bounding box du personnage
    glm::vec3 minBB = character.getMinBoundingBox();
    glm::vec3 maxBB = character.getMaxBoundingBox();

// Parcours des blocs proches de la bounding box du personnage
    for (int x = static_cast<int>(minBB.x); x <= static_cast<int>(maxBB.x); ++x) {
        for (int y = static_cast<int>(minBB.y); y <= static_cast<int>(maxBB.y); ++y) {
            for (int z = static_cast<int>(minBB.z); z <= static_cast<int>(maxBB.z); ++z) {
                // Vérification des blocs dans le chunk actuel et le chunk proche
                if ((chunk && chunk->getBloc(x, y, z) != AIR)
//                ||
                    //      (chunkProche
                    // && chunkProche->getBloc(x, y, z) != AIR)
                        )
                {
//                    std::cout << "colision avec un bloc de type " << chunkProche->getBloc(x, y, z) << std::endl;
                    resolveCollisionForBlock(character, glm::vec3(x, y, z));
                }
            }
        }
    }

// Application du mouvement du personnage
    character.move(character.vecteurDirection);
}