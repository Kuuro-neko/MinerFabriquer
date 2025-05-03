#include <TP/Scene/World.hpp>
#include <random>
#include "WorldGenerator.hpp"
#include "TP/Character/Character.hpp"
#include <algorithm>
#include "World.hpp"
#include <unordered_set>

#include <chrono>
#include <iostream>

std::set<std::pair<int, int>> World::getDirtyColumns()
{
    std::set<std::pair<int, int>> dirtyColumns;
    for (auto &[key, column]: chunkColumns) {
        if (column.isDirty()) {
            dirtyColumns.insert({key.x, key.y});
        }
    }
    return dirtyColumns;
}

void World::updateSkyLightsInColumn(int x, int z)
{
    ChunkColumn *column = getChunkColumn(x, z);
    if (!column) return;
    
    column->updateSkyLights();

    std::vector<std::vector<int>> *surfaceHeightmap = column->getSurfaceHeightMap();

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            int y = (*surfaceHeightmap)[x][z];
            if (y != -1) {
                VoxelChunk *chunk = column->getChunkContainingHeight(y);
                if(chunk) lightFloodfill(
                    x + chunk->m_chunkCoords.x * CHUNK_SIZE,
                    y + chunk->m_chunkCoords.y * CHUNK_SIZE + 1,
                    z + chunk->m_chunkCoords.z * CHUNK_SIZE,
                    MAX_LIGHT
                );
            }
        }
    }
}

World::World() : SceneNode(Transform(), new MeshObject(), nullptr)
{
    generation();
}

World::~World() {
}

VoxelChunk *World::createEmptyChunk(int x, int y, int z) {
    ChunkColumn *column = getChunkColumn(x, z);
    if (!column) {
        chunkColumns.emplace(glm::ivec2(x, z), ChunkColumn(x, z));
    }
    column = getChunkColumn(x, z);
    chunks.emplace(glm::ivec3(x, y, z), VoxelChunk(CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE));
    VoxelChunk *chunk = &chunks.at(glm::ivec3(x, y, z));
    chunk->translate(glm::vec3(x * CHUNK_SIZE, y * CHUNK_SIZE, z * CHUNK_SIZE));
    chunk->m_world = this;
    chunk->m_chunkCoords = glm::ivec3(x, y, z);
    column->addChunk(chunk);
    return chunk;
}

void World::removeChunkColumn(int x, int z) {
    ChunkColumn *column = getChunkColumn(x, z);
    if (!column) {
        return;
    }
    column->free();
    auto it = chunkColumns.find({x, z});
    if (it != chunkColumns.end()) {
        chunkColumns.erase(it);
    }
}

ChunkColumn *World::getChunkColumn(int x, int z) {
    auto it = chunkColumns.find({x, z});
    if (it != chunkColumns.end()) {
        return &it->second;
    }
    return nullptr;
}

VoxelChunk *World::getChunk(int x, int y, int z) {
    ChunkColumn *column = getChunkColumn(x, z);
    if (column) {
        return column->getChunk(y);
    }
    return nullptr;
}

int World::playerRemoveBlock(int x, int y, int z, unsigned char gamemode) {
    VoxelChunk *chunk = getChunkContaining(x, y, z);
    ChunkColumn *column = getChunkColumn(chunk->m_chunkCoords.x, chunk->m_chunkCoords.z);
    if (!column) return -1;
    if (chunk) {
        int bloc = chunk->playerRemoveBlock(x % CHUNK_SIZE, y % CHUNK_SIZE, z % CHUNK_SIZE, gamemode);
        if (bloc == -1) return bloc;
        std::vector<std::vector<int>> *surfaceHeightmap = column->getSurfaceHeightMap();
        int currentSurface = surfaceHeightmap->at(x % CHUNK_SIZE).at(z % CHUNK_SIZE);
        if (y == currentSurface) {
            // Skylight is now available so we update it
            column->updateSkyLights(x % CHUNK_SIZE, z % CHUNK_SIZE);
            int newSurface = surfaceHeightmap->at(x % CHUNK_SIZE).at(z % CHUNK_SIZE);
            for (int i = currentSurface; i > newSurface; i--) {
                lightFloodfill(x, i, z, MAX_LIGHT);
            }
        } else {
            // Update the light levels of the neighbors
            updateLightFloodfill(x+1, y, z);
            updateLightFloodfill(x-1, y, z);
            updateLightFloodfill(x, y+1, z);
            updateLightFloodfill(x, y-1, z);
            updateLightFloodfill(x, y, z+1);
            updateLightFloodfill(x, y, z-1);
        }
        return bloc;
    }
    return -1;
}

int World::removeBlock(int x, int y, int z) {
    VoxelChunk *chunk = getChunkContaining(x, y, z);
    ChunkColumn *column = getChunkColumn(chunk->m_chunkCoords.x, chunk->m_chunkCoords.z);
    if (!column) return -1;
    if (chunk) {
        int bloc = chunk->removeBlock(x % CHUNK_SIZE, y % CHUNK_SIZE, z % CHUNK_SIZE);
        if (bloc == -1) return bloc;
        std::vector<std::vector<int>> *surfaceHeightmap = column->getSurfaceHeightMap();
        int currentSurface = surfaceHeightmap->at(x % CHUNK_SIZE).at(z % CHUNK_SIZE);
        if (y == currentSurface) {
            // Skylight is now available so we update it
            column->updateSkyLights(x % CHUNK_SIZE, z % CHUNK_SIZE);
            int newSurface = surfaceHeightmap->at(x % CHUNK_SIZE).at(z % CHUNK_SIZE);
            for (int i = currentSurface; i > newSurface; i--) {
                lightFloodfill(x, i, z, MAX_LIGHT);
            }
        } else {
            // Update the light levels of the neighbors
            updateLightFloodfill(x+1, y, z);
            updateLightFloodfill(x-1, y, z);
            updateLightFloodfill(x, y+1, z);
            updateLightFloodfill(x, y-1, z);
            updateLightFloodfill(x, y, z+1);
            updateLightFloodfill(x, y, z-1);
        }
        return bloc;
    }
    return -1;
}

bool World::setBloc(int x, int y, int z, int bloc) {
    VoxelChunk *chunk = getChunkContaining(x, y, z);
    if (chunk) {
        bool err = chunk->setBloc(x % CHUNK_SIZE, y % CHUNK_SIZE, z % CHUNK_SIZE, bloc);
        if (!err) {
            ChunkColumn *column = getChunkColumn(chunk->m_chunkCoords.x, chunk->m_chunkCoords.z);
            auto& heightmap = column->getSurfaceHeightMap()->at(x % CHUNK_SIZE).at(z % CHUNK_SIZE);
            if (y > heightmap) {
                for (int i = heightmap; i < y; i++) {
                    setLightLevel(x, i, z, MIN_LIGHT);
                    // Update the light levels of the neighbors
                    updateLightFloodfill(x+1, y, z);
                    updateLightFloodfill(x-1, y, z);
                    updateLightFloodfill(x, y, z+1);
                    updateLightFloodfill(x, y, z-1);
                }
                heightmap = y;
            } else {
                updateLightFloodfill(x+1, y, z);
                updateLightFloodfill(x-1, y, z);
                updateLightFloodfill(x, y+1, z);
                updateLightFloodfill(x, y-1, z);
                updateLightFloodfill(x, y, z+1);
                updateLightFloodfill(x, y, z-1);
            }

            int emission = BlocDatabase::getInstance().defaultLightLevel(bloc);
            if (emission) doLightFloodFillNeighbors(x, y, z, emission-1);
        }
        return err;
    }
    return false;
}

unsigned short World::getLightLevel(int x, int y, int z) {
    VoxelChunk *chunk = getChunkContaining(x, y, z);
    if (chunk) {
        return chunk->getLightLevelIncludingNeighbors(x % CHUNK_SIZE, y % CHUNK_SIZE, z % CHUNK_SIZE);
    } else {
        return MAX_LIGHT;
    }
}

int World::getBloc(int x, int y, int z) {
    VoxelChunk *chunk = getChunkContaining(x, y, z);
    if (chunk) {
        return chunk->getBloc(x % CHUNK_SIZE, y % CHUNK_SIZE, z % CHUNK_SIZE);
    } else {
        return OUT_OF_BOUNDS_BLOC;
    }
}

VoxelChunk *World::getChunkContaining(int x, int y, int z) {
    int chunkCoordX = (x < 0) ? (x - CHUNK_SIZE + 1) / CHUNK_SIZE : x / CHUNK_SIZE;
    int chunkCoordY = (y < 0) ? (y - CHUNK_SIZE + 1) / CHUNK_SIZE : y / CHUNK_SIZE;
    int chunkCoordZ = (z < 0) ? (z - CHUNK_SIZE + 1) / CHUNK_SIZE : z / CHUNK_SIZE;
    ChunkColumn *column = getChunkColumn(chunkCoordX, chunkCoordZ);
    if (!column) {
        return nullptr;
    }
    return column->getChunk(chunkCoordY);
}

VoxelChunk *World::getChunkContaining(glm::vec3 position) {
    int x = static_cast<int>(position.x);
    int y = static_cast<int>(position.y);
    int z = static_cast<int>(position.z);
    return getChunkContaining(x, y, z);
}

std::vector<VoxelChunk *> World::getAllChunks() {
    std::vector<VoxelChunk *> allChunks;
    for (auto &[key, column]: chunkColumns) {
        std::vector<VoxelChunk *> chunks = column.getChunks();
        for (auto &chunk: chunks) {
            allChunks.push_back(chunk);
        }
    }
    return allChunks;
}

std::vector<VoxelChunk *> World::getIntersectedChunks(Ray ray, float maxDistance) {
    std::vector<VoxelChunk *> intersectedChunks;
    std::vector<VoxelChunk *> chunks = getAllChunks();
    for (auto &chunk: chunks) {
        if (chunk->intersects(ray, maxDistance)) {
            intersectedChunks.push_back(chunk);
        }
    }
    return intersectedChunks;
}

void World::draw(GLuint programID) {

    std::set<std::pair<int, int>> dirtyColumns = getDirtyColumns();
    // update lights for each dirtyColumn, ITERATE THROUGH THE SET OMG !!
    for (auto &[x, z]: dirtyColumns) {
        updateSkyLightsInColumn(x, z);
    }

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
    // Generate the world
    std::cout << "Generating world... 0%" << std::flush;
    auto start = std::chrono::high_resolution_clock::now();
    for (int x = 0; x <= GENERATION_SIZE_X; ++x) {
        for (int y = 0; y <= GENERATION_SIZE_Y; ++y) {
            for (int z = 0; z <= GENERATION_SIZE_Z; ++z) {
                VoxelChunk *chunk = createEmptyChunk(x, y, z);
                worldGenerator.genereteProceduralChunk(chunk, x, y, z);
            }
        }
        std::cout << "\rGenerating world... " << int((x * 100) / GENERATION_SIZE_X) << "%" << std::flush;
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "\rGenerating world...  done ! (" << ms << " ms) \n";

    // Update all lights levels
    //  ->  First set the sky lights to 15 for all air blocks
    std::cout << "Updating lights... " << std::flush;
    start = std::chrono::high_resolution_clock::now();
    for (int x = 0; x <= GENERATION_SIZE_X; ++x) {
        for (int z = 0; z <= GENERATION_SIZE_Z; ++z) {
            updateSkyLightsInColumn(x, z);
        }
    }
    //  ->  Then floodfill the lights
    for (auto column : chunkColumns) {
        for (auto chunk : column.second.getChunks()) {
            glm::ivec3 chunkCoords = chunk->m_chunkCoords;
            for (int i = 0; i < CHUNK_SIZE; ++i) {
                for (int k = 0; k < CHUNK_SIZE; ++k) {
                    // Bloc column is completely above the surface, skip it
                    if(chunk->m_chunkCoords.y * CHUNK_SIZE >= column.second.getSurfaceHeightMap()->at(i).at(k)) continue;
                    for (int j = CHUNK_SIZE - 1; j >= 0; --j) {
                        int lightLevel = chunk->m_lights[i][j][k];
                        if (lightLevel < 15) continue;
        
                        int worldX = i + chunkCoords.x * CHUNK_SIZE;
                        int worldY = j + chunkCoords.y * CHUNK_SIZE;
                        int worldZ = k + chunkCoords.z * CHUNK_SIZE;
        
                        lightFloodfill(worldX, worldY, worldZ, lightLevel);
                    }
                }
            }
        }
    }
    end = std::chrono::high_resolution_clock::now();
    ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "  done ! (" << ms << " ms) \n";

    // Generate meshes for first draw calls
    std::cout << "Generating meshes..." << std::flush;
    std::vector<VoxelChunk *> chunks = getAllChunks();
    for (auto chunk : chunks) {
        chunk->generateMesh();
    }
    std::cout << " done !" << std::endl;
}

void World::cleanupBuffers() {
    std::vector<VoxelChunk *> chunks = getAllChunks();
    for (auto chunk : chunks) {
        chunk->cleanupBuffers();
    }
}

void World::updateVisibleChunk(Frustrum &frustum) {

    // on parcourt tous les chunks et on les ajoute à la liste des chunks visibles s'ils sont dans le frustum
    std::vector<VoxelChunk *> chunks = getAllChunks();
    for (auto chunk: chunks) {
        if (frustum.isBoundingBoxInFrustum(chunk->getWorldPosition(),
                                           chunk->getWorldPosition() + glm::vec3(CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE))) {
            visibleChunks[chunk->m_chunkCoords] = chunk;
        } else {
            // si le chunk n'est pas visible, on le supprime de la liste des chunks visibles
            auto it = visibleChunks.find(chunk->m_chunkCoords);
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

void World::updateLightFloodfill(int x, int y, int z) {
    VoxelChunk *chunk = getChunkContaining(x, y, z);
    if (chunk == nullptr) return;
    lightFloodfill(x, y, z, chunk->m_lights[x % CHUNK_SIZE][y % CHUNK_SIZE][z % CHUNK_SIZE]);
}

void World::doLightFloodFillNeighbors(int x, int y, int z, int lightLevel) {
    lightFloodfill(x + 1, y, z, lightLevel);
    lightFloodfill(x - 1, y, z, lightLevel);
    lightFloodfill(x, y + 1, z, lightLevel);
    lightFloodfill(x, y - 1, z, lightLevel);
    lightFloodfill(x, y, z + 1, lightLevel);
    lightFloodfill(x, y, z - 1, lightLevel);
}

void World::lightFloodfill(int startX, int startY, int startZ, int startLightLevel) {
    if (startLightLevel <= 0) return;
   

    std::queue<std::tuple<int, int, int, int>> queue; // (x, y, z, lightLevel)
    std::unordered_set<uint64_t> visited;

    auto encodePos = [](int x, int y, int z) -> uint64_t {
        return (static_cast<uint64_t>(x) & 0x3FFFFF) << 42 |
               (static_cast<uint64_t>(y) & 0x3FFFFF) << 21 |
               (static_cast<uint64_t>(z) & 0x1FFFFF);
    };

    queue.emplace(startX, startY, startZ, startLightLevel);

    while (!queue.empty()) {
        auto [x, y, z, lightLevel] = queue.front();
        queue.pop();

        if (lightLevel <= 0) continue;

        // Position déjà visitée à une intensité supérieure ou égale
        uint64_t key = encodePos(x, y, z);
        if (!visited.insert(key).second) continue;
        
        VoxelChunk *chunk = getChunkContaining(x, y, z);
        if (chunk == nullptr) continue;
        
        int localX = x % CHUNK_SIZE;
        int localY = y % CHUNK_SIZE;
        int localZ = z % CHUNK_SIZE;
        if (localX < 0) localX += CHUNK_SIZE;
        if (localY < 0) localY += CHUNK_SIZE;
        if (localZ < 0) localZ += CHUNK_SIZE;

        int bloc = chunk->getBloc(localX, localY, localZ);

        if (BlocDatabase::getInstance().isOpaque(bloc)) continue;

        int currentLight = chunk->m_lights[localX][localY][localZ];
        //std::cout << "Light floodfill at " << x << ", " << y << ", " << z << " with level " << lightLevel << " Current is " << currentLight << std::endl;
        if (currentLight > lightLevel) continue;
        if (currentLight < lightLevel) chunk->dirty = true;
        
        chunk->m_lights[localX][localY][localZ] = lightLevel;

        // Propagation dans les 6 directions
        queue.emplace(x + 1, y, z, lightLevel - 1);
        queue.emplace(x - 1, y, z, lightLevel - 1);
        queue.emplace(x, y + 1, z, lightLevel - 1);
        queue.emplace(x, y - 1, z, lightLevel - 1);
        queue.emplace(x, y, z + 1, lightLevel - 1);
        queue.emplace(x, y, z - 1, lightLevel - 1);
    }
}

void World::setLightLevel(int x, int y, int z, int lightLevel)
{
    VoxelChunk *chunk = getChunkContaining(x, y, z);
    if (chunk == nullptr) return;
    int localX = x % CHUNK_SIZE;
    int localY = y % CHUNK_SIZE;
    int localZ = z % CHUNK_SIZE;
    if (localX < 0) localX += CHUNK_SIZE;
    if (localY < 0) localY += CHUNK_SIZE;
    if (localZ < 0) localZ += CHUNK_SIZE;

    chunk->m_lights[localX][localY][localZ] = lightLevel;
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