#include <TP/Scene/World.hpp>
#include <random>
#include "WorldGenerator.hpp"
#include "TP/Character/Character.hpp"
#include <algorithm>
#include "World.hpp"

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

int World::playerRemoveBlock(int x, int y, int z, unsigned char gamemode) {
    VoxelChunk *chunk = getChunkAt(x, y, z);
    if (chunk) {
        return chunk->playerRemoveBlock(x % CHUNK_SIZE, y % CHUNK_SIZE, z % CHUNK_SIZE, gamemode);
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
    glEnable(GL_CULL_FACE);
    for (auto &[key, chunk]: visibleChunks) {
        //si la distance de rendu est depassee, on ne dessine pas le chunk
        float distance = glm::length(chunk->getWorldPosition() - camera->getPosition());
        if (distance <= (RENDERER_DISTANCE * CHUNK_SIZE)) {
            chunk->draw(programID);
        }
    }
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for (auto &[key, chunk]: visibleChunks) {
        //si la distance de rendu est depassee, on ne dessine pas le chunk
        float distance = glm::length(chunk->getWorldPosition() - camera->getPosition());
        if (distance <= (RENDERER_DISTANCE * CHUNK_SIZE)) {
            chunk->drawTransparent(programID);
        }
    }
    glDisable(GL_BLEND);
    
}

void World::generation() {
    WorldGenerator worldGenerator;
    std::cout << "Generating world... 0%" << std::flush;
    // 2x2 chunks for testing
    for (int x = 0; x <= GENERATION_SIZE_X; ++x) {
        for (int y = 0; y <= GENERATION_SIZE_Y; ++y) {
            for (int z = 0; z <= GENERATION_SIZE_Z; ++z) {
                VoxelChunk *chunk = createEmptyChunk(x, y, z);
                worldGenerator.genereteProceduralChunk(chunk, x, y, z);
            }
        }
        std::cout << "\rGenerating world... " << int((x * 100) / 32) << "%" << std::flush;
    }
    std::cout << "\rGenerating world... done !" << std::endl;
    std::cout << "Generating meshes..." << std::flush;
    // generate all meshes
    for (auto &[key, chunk]: chunks) {
        chunk.generateMesh();
    }
    std::cout << " done !" << std::endl;
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
            visibleChunks[key] = &chunk;
        } else {
            // si le chunk n'est pas visible, on le supprime de la liste des chunks visibles
            auto it = visibleChunks.find(key);
            if (it != visibleChunks.end()) {
                it->second->cleanupBuffers();
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

void World::update(float deltaTime) {
    if (doDaylightCycle) time += deltaTime;
}
    // Récupération de la position du personnage
void World::resolveCollisions(Character &character, World *world) {
    if (character.getGamemode() == GAMEMODE_SPECTATOR) {
        character.move(character.vecteurDirection);
        return;
    }
    //TODO au lieu de faire la vérification dans world, on fait un broadphase -> on vérifie quel chunk
    // sont intersecté par le personnage et on fait la recherche dans ces deux chunks
    // On peut aussi ajouter un octree pour optimiser la recherche
    glm::vec3 characterPosition = character.getWorldPosition();


// Récupération de la bounding box du personnage
    glm::vec3 minBB = character.getMinBoundingBox();
    glm::vec3 maxBB = character.getMaxBoundingBox();

// Parcours des blocs proches de la bounding box du personnage
    for (int x = static_cast<int>(minBB.x); x <= static_cast<int>(maxBB.x); ++x) {
        for (int y = static_cast<int>(minBB.y); y <= static_cast<int>(maxBB.y); ++y) {
            for (int z = static_cast<int>(minBB.z); z <= static_cast<int>(maxBB.z); ++z) {
                // Vérification des blocs dans le chunk actuel et le chunk proche
                if(world->getBloc(x, y, z) != AIR) {
                    resolveCollisionForBlock(character, glm::vec3(x, y, z));
                }
            }
        }
    }


    // Application du mouvement du personnage
    character.move(character.vecteurDirection);
}