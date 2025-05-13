#include <TP/Scene/World.hpp>
#include <random>
#include "WorldGenerator.hpp"
#include "TP/Character/Character.hpp"
#include <algorithm>
#include "World.hpp"
#include <unordered_set>
#include <TP/Entities/Zombie.hpp>
#include <TP/Entities/Projectiles/TNTProjectile.hpp>
#include <TP/Entities/Projectiles/EnderPearl.hpp>
#include <TP/Textures/TextureManager.hpp>
#include "MobSpawner.hpp"
#include <chrono>
#include <iostream>

void World::emplaceChunk(std::shared_ptr<VoxelChunk> &chunk)
{
    chunks.emplace(chunk->m_chunkCoords, chunk);
}

void World::workerLoop()
{
    while (workerThreadRunning)
    {
        std::unique_lock<std::mutex> lock(taskQueueMutex);
        taskQueueCV.wait(lock, [&]
                         { return !taskQueue.empty() || !workerThreadRunning; });

        if (!workerThreadRunning)
            break;

        auto task = taskQueue.front();
        taskQueue.pop();
        lock.unlock();

        if (task.taskType == TASK_GENERATION)
        {
            {
                // check if column exists
                std::unique_lock<std::recursive_mutex> worldLock(worldMutex);
                auto it = chunkColumns.find(glm::ivec2(task.x, task.z));
                if (it != chunkColumns.end())
                {
                    // Column already exists, skip generation
                    continue;
                }
            }

            auto newColumn = std::make_shared<ChunkColumn>(task.x, task.z);
            for (int y = 0; y <= GENERATION_SIZE_Y; ++y)
            {
                auto newChunk = std::make_shared<VoxelChunk>();
                newChunk->translate(glm::vec3(task.x * CHUNK_SIZE, y * CHUNK_SIZE, task.z * CHUNK_SIZE));
                newChunk->m_chunkCoords = glm::ivec3(task.x, y, task.z);
                newColumn->addChunk(newChunk);
            }

            newColumn->generate();

            {
                std::unique_lock<std::recursive_mutex> worldLock(worldMutex);
                chunkColumns.emplace(glm::ivec2(task.x, task.z), newColumn);
                newColumn->assignWorld(this);
                for (auto &chunk : newColumn->getChunks())
                {
                    emplaceChunk(chunk);
                    chunk->dirty = true;
                }
                // generateChunkColumn(x, z);  // Actual column generation
            }
        }
        else if (task.taskType == TASK_SUPPRESSION)
        {
            {
                std::unique_lock<std::recursive_mutex> worldLock(worldMutex);
                removeChunkColumn(task.x, task.z); // Actual column suppression
            }
        }
    }
}

void World::enqueueChunkGeneration(int x, int z)
{
    {
        std::lock_guard<std::mutex> lock(taskQueueMutex);
        taskQueue.push({x, z, TASK_GENERATION});
    }
    taskQueueCV.notify_one();
}

void World::enqueueColumnSuppression(int x, int z)
{
    {
        std::lock_guard<std::mutex> lock(taskQueueMutex);
        taskQueue.push({x, z, TASK_SUPPRESSION});
    }
    taskQueueCV.notify_one();
}

void World::startWorkerThread()
{
    workerThreads.emplace_back(std::thread(&World::workerLoop, this));
    workerThreads.emplace_back(std::thread(&World::workerLoop, this));
    workerThreads.emplace_back(std::thread(&World::workerLoop, this));
}

void World::stopWorkerThread()
{
    workerThreadRunning = false;
    taskQueueCV.notify_all();
    for (auto &thread : workerThreads)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }
}

World::World() : SceneNode(Transform(), new MeshObject(), nullptr)
{
    m_mobSpawner = new MobSpawner(this);
}

World::~World()
{
    stopWorkerThread();
}

std::vector<std::shared_ptr<ChunkColumn>> World::getAllColumns() const
{
    std::vector<std::shared_ptr<ChunkColumn>> columns;
    for (auto &[key, column] : chunkColumns)
    {
        columns.push_back(column);
    }
    return columns;
}

std::set<std::pair<int, int>> World::getDirtyColumns()
{
    std::set<std::pair<int, int>> dirtyColumns;
    for (auto &[key, column] : chunkColumns)
    {
        if (column->isSkylightDirty())
        {
            dirtyColumns.insert({key.x, key.y});
        }
    }
    return dirtyColumns;
}

void World::updateSkyLightsInColumn(int x, int z)
{
    std::shared_ptr<ChunkColumn> column = getChunkColumn(x, z);
    if (!column)
        return;

    column->updateSkyLights();

    std::vector<std::vector<int>> *surfaceHeightmap = column->getSurfaceHeightMap();

    for (int x = 0; x < CHUNK_SIZE; x++)
    {
        for (int z = 0; z < CHUNK_SIZE; z++)
        {
            int y = (*surfaceHeightmap)[x][z];
            if (y != -1)
            {
                lightFloodfill(
                    x + column->getChunkCoords().x * CHUNK_SIZE,
                    y + 1,
                    z + column->getChunkCoords().y * CHUNK_SIZE,
                    MAX_LIGHT);
            }
        }
    }

    column->markSkylightDirty(false);
}

void World::addColumn(std::shared_ptr<ChunkColumn> column)
{
    chunkColumns.emplace(column->getChunkCoords(), column);
    for (auto &chunk : column->getChunks())
    {
        chunks.emplace(glm::ivec3(chunk->m_chunkCoords.x, chunk->m_chunkCoords.y, chunk->m_chunkCoords.z), chunk);
    }
}

void World::removeChunkColumn(int x, int z)
{
    std::shared_ptr<ChunkColumn> column = getChunkColumn(x, z);
    if (!column)
    {
        return;
    }
    for (std::shared_ptr<VoxelChunk> chunk : column->getChunks())
    {
        chunks.erase(glm::ivec3(chunk->m_chunkCoords.x, chunk->m_chunkCoords.y, chunk->m_chunkCoords.z));
    }
    column->free();
    chunkColumns.erase(glm::ivec2(x, z));
}

std::shared_ptr<ChunkColumn> World::getChunkColumn(int x, int z) const
{
    auto it = chunkColumns.find({x, z});
    if (it != chunkColumns.end() && it->second)
    {
        return it->second;
    }
    // std::cerr << "[getChunkColumn] ChunkColumn at (" << x << ", " << z << ") is invalid or not found" << std::endl;
    return nullptr;
}

std::shared_ptr<VoxelChunk> World::getChunk(int x, int y, int z)
{
    std::shared_ptr<ChunkColumn> column = getChunkColumn(x, z);
    if (column)
    {
        return column->getChunk(y);
    }
    return nullptr;
}

int World::playerRemoveBlock(int x, int y, int z, unsigned char gamemode)
{
    if(y < 0) return -1;
    std::shared_ptr<VoxelChunk> chunk = getChunkContaining(x, y, z);
    if(!chunk)
        return -1;
    std::shared_ptr<ChunkColumn> column = getChunkColumn(chunk->m_chunkCoords.x, chunk->m_chunkCoords.z);
    if (!column)
        return -1;
    if (chunk)
    {
        glm::ivec3 localCoords = glm::ivec3(
            betterModulo(x, CHUNK_SIZE),
            betterModulo(y, CHUNK_SIZE),
            betterModulo(z, CHUNK_SIZE));
        int bloc = chunk->playerRemoveBlock(localCoords.x, localCoords.y, localCoords.z, gamemode);
        if (bloc == -1)
            return bloc;
        std::vector<std::vector<int>> *surfaceHeightmap = column->getSurfaceHeightMap();
        int currentSurface = surfaceHeightmap->at(localCoords.x).at(localCoords.z);
        if (y == currentSurface)
        {
            // Skylight is now available so we update it
            column->updateSkyLights(localCoords.x, localCoords.z);
            int newSurface = surfaceHeightmap->at(localCoords.x).at(localCoords.z);
            for (int i = currentSurface; i > newSurface; i--)
            {
                lightFloodfill(x, i, z, MAX_LIGHT);
            }
        }
        else
        {
            // Update the light levels of the neighbors
            updateLightFloodfill(x + 1, y, z);
            updateLightFloodfill(x - 1, y, z);
            updateLightFloodfill(x, y + 1, z);
            updateLightFloodfill(x, y - 1, z);
            updateLightFloodfill(x, y, z + 1);
            updateLightFloodfill(x, y, z - 1);
        }
        return bloc;
    }
    return -1;
}

int World::removeBlock(int x, int y, int z)
{
    if(y < 0) return -1;
    std::shared_ptr<VoxelChunk> chunk = getChunkContaining(x, y, z);
    if(!chunk)
        return -1;
    std::shared_ptr<ChunkColumn> column = getChunkColumn(chunk->m_chunkCoords.x, chunk->m_chunkCoords.z);
    if (!column)
        return -1;
    if (chunk)
    {
        glm::ivec3 localCoords = glm::ivec3(
            betterModulo(x, CHUNK_SIZE),
            betterModulo(y, CHUNK_SIZE),
            betterModulo(z, CHUNK_SIZE));
        int bloc = chunk->removeBlock(localCoords.x, localCoords.y, localCoords.z);
        if (bloc == -1)
            return bloc;
        std::vector<std::vector<int>> *surfaceHeightmap = column->getSurfaceHeightMap();
        int currentSurface = surfaceHeightmap->at(localCoords.x).at(localCoords.z);
        if (y == currentSurface)
        {
            // Skylight is now available so we update it
            column->updateSkyLights(localCoords.x, localCoords.z);
            int newSurface = surfaceHeightmap->at(localCoords.x).at(localCoords.z);
            for (int i = currentSurface; i > newSurface; i--)
            {
                lightFloodfill(x, i, z, MAX_LIGHT);
            }
        }
        else
        {
            // Update the light levels of the neighbors
            updateLightFloodfill(x + 1, y, z);
            updateLightFloodfill(x - 1, y, z);
            updateLightFloodfill(x, y + 1, z);
            updateLightFloodfill(x, y - 1, z);
            updateLightFloodfill(x, y, z + 1);
            updateLightFloodfill(x, y, z - 1);
        }
        return bloc;
    }
    return -1;
}

bool World::setBloc(int x, int y, int z, int bloc)
{
    if(y < 0) return false;
    std::shared_ptr<VoxelChunk> chunk = getChunkContaining(x, y, z);
    if (chunk)
    {
        glm::ivec3 localCoords = glm::ivec3(
            betterModulo(x, CHUNK_SIZE),
            betterModulo(y, CHUNK_SIZE),
            betterModulo(z, CHUNK_SIZE));
        bool err = chunk->setBloc(localCoords.x, localCoords.y, localCoords.z, bloc, true, false);
        if (!err)
        {
            std::shared_ptr<ChunkColumn> column = getChunkColumn(chunk->m_chunkCoords.x, chunk->m_chunkCoords.z);
            auto &heightmap = column->getSurfaceHeightMap()->at(localCoords.x).at(localCoords.z);
            if (y > heightmap)
            {
                for (int i = heightmap; i < y; i++)
                {
                    setLightLevel(x, i, z, MIN_LIGHT);
                    // Update the light levels of the neighbors
                    updateLightFloodfill(x + 1, y, z);
                    updateLightFloodfill(x - 1, y, z);
                    updateLightFloodfill(x, y, z + 1);
                    updateLightFloodfill(x, y, z - 1);
                }
                heightmap = y;
            }
            else
            {
                updateLightFloodfill(x + 1, y, z);
                updateLightFloodfill(x - 1, y, z);
                updateLightFloodfill(x, y + 1, z);
                updateLightFloodfill(x, y - 1, z);
                updateLightFloodfill(x, y, z + 1);
                updateLightFloodfill(x, y, z - 1);
            }

            int emission = BlocDatabase::getInstance().defaultLightLevel(bloc);
            if (emission)
                lightFloodFillNeighbors(x, y, z, emission - 1);
        }
        return err;
    }
    return false;
}

bool World::generationSetBloc(int x, int y, int z, int bloc)
{
    if (y < 0) return false;
    std::shared_ptr<VoxelChunk> chunk = getChunkContaining(x, y, z);
    if (chunk)
    {
        glm::ivec3 localCoords = glm::ivec3(
            betterModulo(x, CHUNK_SIZE),
            betterModulo(y, CHUNK_SIZE),
            betterModulo(z, CHUNK_SIZE));
        bool err = chunk->generationSetBloc(localCoords.x, localCoords.y, localCoords.z, bloc);
        return err;
    }
    return false;
}

unsigned short World::getLightLevel(int x, int y, int z)
{
    std::shared_ptr<VoxelChunk> chunk = getChunkContaining(x, y, z);
    if (chunk)
    {
        return chunk->getLightLevel(betterModulo(x, CHUNK_SIZE), betterModulo(y, CHUNK_SIZE), betterModulo(z, CHUNK_SIZE));
    }
    else
    {
        return MAX_LIGHT;
    }
}

int World::getBloc(int x, int y, int z)
{
    if (y < 0) return OUT_OF_BOUNDS_BLOC;
    std::shared_ptr<VoxelChunk> chunk = getChunkContaining(x, y, z);
    if (chunk)
    {
        return chunk->getBloc(betterModulo(x, CHUNK_SIZE), betterModulo(y, CHUNK_SIZE), betterModulo(z, CHUNK_SIZE), true, false);
    }
    else
    {
        // std::cout << "chunk not found at " << x << ", " << y << ", " << z << " in world.getBloc" << std::endl;
        return OUT_OF_BOUNDS_BLOC;
    }
}

std::shared_ptr<VoxelChunk> World::getChunkContaining(int x, int y, int z) const
{
    if (y < 0) return nullptr;
    int chunkCoordX = (x < 0) ? (x - CHUNK_SIZE + 1) / CHUNK_SIZE : x / CHUNK_SIZE;
    int chunkCoordY = (y < 0) ? (y - CHUNK_SIZE + 1) / CHUNK_SIZE : y / CHUNK_SIZE;
    int chunkCoordZ = (z < 0) ? (z - CHUNK_SIZE + 1) / CHUNK_SIZE : z / CHUNK_SIZE;

    std::shared_ptr<ChunkColumn> column = getChunkColumn(chunkCoordX, chunkCoordZ);
    if (!column)
    {
        // std::cout << "Chunk coords : " << chunkCoordX << ", " << chunkCoordY << ", " << chunkCoordZ << std::endl;
        return nullptr;
    }
    return column->getChunkContainingHeight(y);
}

std::shared_ptr<VoxelChunk> World::getChunkContaining(glm::vec3 position) const
{
    if (position.y < 0) return nullptr;
    int x = static_cast<int>(position.x);
    int y = static_cast<int>(position.y);
    int z = static_cast<int>(position.z);
    return getChunkContaining(x, y, z);
}

std::vector<std::shared_ptr<VoxelChunk>> World::getAllChunks()
{
    std::vector<std::shared_ptr<VoxelChunk>> allChunks;
    for (auto &[key, column] : chunkColumns)
    {
        std::vector<std::shared_ptr<VoxelChunk>> chunks = column->getChunks();
        for (auto &chunk : chunks)
        {
            allChunks.push_back(chunk);
        }
    }
    return allChunks;
}

std::vector<std::shared_ptr<VoxelChunk>> World::getIntersectedChunks(Ray ray, float maxDistance)
{
    std::vector<std::shared_ptr<VoxelChunk>> intersectedChunks;
    for (const auto &[key, chunk] : chunks)
    {
        if (!chunk)
            continue;
        if (chunk->intersects(ray, maxDistance))
        {
            intersectedChunks.push_back(chunk);
        }
    }
    return intersectedChunks;
}

void World::updateLoadedChunks()
{
    glm::vec3 pos = camera->getPosition();
    std::shared_ptr<VoxelChunk> chunk = getChunkContaining(pos);
    if (!chunk)
    {
        // std::cout << "No chunk found at camera position" << std::endl;
        return;
    }
    int chunkX = chunk->m_chunkCoords.x;
    int chunkY = chunk->m_chunkCoords.y;
    int chunkZ = chunk->m_chunkCoords.z;

    // look for all the columns around, and remove those too far.
    std::vector<glm::ivec2> toRemove;
    for (const auto &[key, column] : chunkColumns)
    {
        if (!column)
            continue; // Ensure the column is valid
        if (key.x == 0 && key.y == 0)
            continue; // Example validation (adjust as needed)
        int distance = std::abs(key.x - chunkX) + std::abs(key.y - chunkZ);
        if (distance > GENERATION_DISTANCE + 4)
        {
            toRemove.push_back(key);
        }
    }
    for (const auto &key : toRemove)
    {
        enqueueColumnSuppression(key.x, key.y);
    }

    // Initialize those not found that should exist.
    for (int x = chunkX - GENERATION_DISTANCE; x <= chunkX + GENERATION_DISTANCE; ++x)
    {
        for (int z = chunkZ - GENERATION_DISTANCE; z <= chunkZ + GENERATION_DISTANCE; ++z)
        {
            std::shared_ptr<ChunkColumn> column = getChunkColumn(x, z);
            int distance = std::abs(x - chunkX) + std::abs(z - chunkZ);
            if (!column && distance < GENERATION_DISTANCE)
            {
                enqueueChunkGeneration(x, z);
            }
        }
    }
    // if(count) std::cout << "Generated " << count << " chunk columns pos(" << chunkX << ", " << chunkZ << ") " << chunks.size() << " chunks in total" << std::endl;
}

void World::draw(GLuint programID)
{
    meshesGenerated = 0;
    glEnable(GL_CULL_FACE);
    if (wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    {
        std::lock_guard<std::recursive_mutex> lock(worldMutex);
        std::set<std::pair<int, int>> dirtyColumns = getDirtyColumns();
        for (auto &[x, z] : dirtyColumns)
        {
            updateSkyLightsInColumn(x, z);
        }

        // int count = 0;
        for (auto &[key, chunk] : visibleChunks)
        {
            if (!chunk)
                continue;
            chunk->drawOpaque(programID);
        }

        // std::cout << "Drawn " << count << " chunks out of " << chunks.size() << std::endl;
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        // sort visible chunks by distance to camera
        std::vector<std::pair<glm::ivec3, std::shared_ptr<VoxelChunk>>> sortedChunks(visibleChunks.begin(), visibleChunks.end());
        std::sort(sortedChunks.begin(), sortedChunks.end(), [this](const auto &a, const auto &b)
                  { return glm::length(a.second->getWorldPosition() - camera->getPosition()) < glm::length(b.second->getWorldPosition() - camera->getPosition()); });
        for (auto &[key, chunk] : sortedChunks)
        {
            if (!chunk)
                continue;
            chunk->drawTransparent(programID);
        }
    }
    if (wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_BLEND);
}

void World::initialGeneration()
{
    int count = 0;
    int total = GENERATION_RADIUS_X * 2;
    auto start = std::chrono::high_resolution_clock::now();
    std::cout << "Generation of the world..." << std::flush;
    for (int x = -GENERATION_RADIUS_X; x <= GENERATION_RADIUS_X; ++x)
    {
        for (int z = -GENERATION_RADIUS_Z; z <= GENERATION_RADIUS_Z; ++z)
        {
            generateChunkColumn(x, z);
        }
        count++;
        std::cout << "\rGeneration of the world... " << int((count * 100) / total) << "%" << std::flush;
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "\rGeneration of the world... done ! (" << ms << " ms) \n";

    // Update all lights levels
    //  ->  First set the sky lights to 15 for all air blocks
    std::cout << "Updating lights... " << std::flush;
    start = std::chrono::high_resolution_clock::now();
    for (int x = -GENERATION_RADIUS_X; x <= GENERATION_RADIUS_X; ++x)
    {
        for (int z = -GENERATION_RADIUS_Z; z <= GENERATION_RADIUS_Z; ++z)
        {
            updateSkyLightsInColumn(x, z);
        }
    }
    //  ->  Then floodfill the lights
    /*for (auto column : chunkColumns) {
        for (auto chunk : column.second->getChunks()) {
            glm::ivec3 chunkCoords = chunk->m_chunkCoords;
            for (int i = 0; i < CHUNK_SIZE; ++i) {
                for (int k = 0; k < CHUNK_SIZE; ++k) {
                    // Bloc column is completely above the surface, skip it
                    if(chunk->m_chunkCoords.y * CHUNK_SIZE >= column.second->getSurfaceHeightMap()->at(i).at(k)) continue;
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
    }*/
    end = std::chrono::high_resolution_clock::now();
    ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "\rUpdating lights... done ! (" << ms << " ms) \n";

    start = std::chrono::high_resolution_clock::now();
    std::cout << "Generation of the meshes..." << std::flush;
    std::vector<std::shared_ptr<VoxelChunk>> chunks = getAllChunks();
    for (auto chunk : chunks)
    {
        chunk->generateMesh();
    }
    end = std::chrono::high_resolution_clock::now();
    ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "\rGeneration of the meshes... done ! (" << ms << " ms) \n";
}

void World::generateChunkColumn(int x, int z)
{
    // Column initialisation
    std::lock_guard<std::recursive_mutex> lock(worldMutex);
    // Column generation
    std::shared_ptr<ChunkColumn> newColumn = std::make_shared<ChunkColumn>(x, z);
    newColumn->generate();
    newColumn->markChunksAsDirty();
    newColumn->markSkylightDirty(true);
    newColumn->assignWorld(this);
    addColumn(newColumn);
}

void World::cleanupBuffers()
{
    std::vector<std::shared_ptr<VoxelChunk>> chunks = getAllChunks();
    for (auto chunk : chunks)
    {
        chunk->cleanupBuffers();
    }
}

void World::updateVisibleChunk(Frustrum &frustum)
{

    // on parcourt tous les chunks et on les ajoute à la liste des chunks visibles s'ils sont dans le frustum
    visibleChunks.clear();
    {
        std::lock_guard<std::recursive_mutex> lock(worldMutex);
        std::vector<std::shared_ptr<VoxelChunk>> chunks = getAllChunks();
        for (const auto &column : chunkColumns)
        {
            std::shared_ptr<ChunkColumn> chunkColumn = column.second;
            int distance = std::abs(chunkColumn->getChunkCoords().x - camera->getPosition().x / CHUNK_SIZE) +
                           std::abs(chunkColumn->getChunkCoords().y - camera->getPosition().z / CHUNK_SIZE);
            if (distance <= RENDERER_DISTANCE)
            {
                for (auto &chunk : chunkColumn->getChunks())
                {
                    if (chunk && frustum.isBoundingBoxInFrustum(chunk->getWorldPosition(),
                                                                chunk->getWorldPosition() + glm::vec3(CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE)))
                    {
                        visibleChunks[chunk->m_chunkCoords] = chunk;
                    }
                }
            }
        }
    }
}

void World::setCamera(Camera &camera)
{
    this->camera = &camera;
}
// function that manage the collision for the block collisiing to hte bounding boxe of the character
void World::resolveCollisionForBlock(Character &character, glm::vec3 blockPosition)
{
    // Bounding box du personnage
    glm::vec3 minBB = character.getMinBoundingBox();
    glm::vec3 maxBB = character.getMaxBoundingBox();

    // Position minimale et maximale du bloc intersecté
    glm::vec3 blockMin = blockPosition;
    glm::vec3 blockMax = blockPosition + glm::vec3(1.0f);

    // Vérification si la bounding box du personnage intersecte celle du bloc
    if (maxBB.x > blockMin.x && minBB.x < blockMax.x &&
        maxBB.y > blockMin.y && minBB.y < blockMax.y &&
        maxBB.z > blockMin.z && minBB.z < blockMax.z)
    {

        // Calcul des overlaps sur chaque axe
        float overlapX = std::min(maxBB.x - blockMin.x, blockMax.x - minBB.x);
        float overlapY = std::min(maxBB.y - blockMin.y, blockMax.y - minBB.y);
        float overlapZ = std::min(maxBB.z - blockMin.z, blockMax.z - minBB.z);

        // Détermination de l'axe avec la plus petite profondeur de collision
        if (overlapX < overlapY && overlapX < overlapZ)
        { // Axe X
            if (character.vecteurDirection.x > 0 && character.getWorldPosition().x < blockPosition.x)
            {
                character.vecteurDirection.x = 0; // Bloque le mouvement vers la droite
            }
            else if (character.vecteurDirection.x < 0 && character.getWorldPosition().x > blockPosition.x)
            {
                character.vecteurDirection.x = 0; // Bloque le mouvement vers la gauche
            }
        }
        else if (overlapY < overlapX && overlapY < overlapZ)
        { // Axe Y
            if (character.vecteurDirection.y > 0 && character.getWorldPosition().y < blockPosition.y)
            {
                character.vecteurDirection.y = 0; // Bloque le mouvement vers le haut
            }
            else if (character.vecteurDirection.y < 0 && character.getWorldPosition().y > blockPosition.y)
            {
                character.vecteurDirection.y = 0; // Bloque le mouvement vers le bas
            }
        }
        else
        { // Axe Z
            if (character.vecteurDirection.z > 0 && character.getWorldPosition().z < blockPosition.z)
            {
                character.vecteurDirection.z = 0; // Bloque le mouvement vers l'avant
            }
            else if (character.vecteurDirection.z < 0 && character.getWorldPosition().z > blockPosition.z)
            {
                character.vecteurDirection.z = 0; // Bloque le mouvement vers l'arrière
            }
        }
    }
}

void World::update(float deltaTime)
{
    for (auto &projectile : projectiles)
    {
        projectile->update(deltaTime);
    }
    if (doDaylightCycle)
        time += deltaTime;
}

void World::updateLightFloodfill(int x, int y, int z)
{
    std::shared_ptr<VoxelChunk> chunk = getChunkContaining(x, y, z);
    if (chunk == nullptr)
        return;
    lightFloodfill(x, y, z, chunk->m_lights[betterModulo(x, CHUNK_SIZE)][betterModulo(y, CHUNK_SIZE)][betterModulo(z, CHUNK_SIZE)]);
}

void World::lightFloodFillNeighbors(int x, int y, int z, int lightLevel)
{
    lightFloodfill(x + 1, y, z, lightLevel);
    lightFloodfill(x - 1, y, z, lightLevel);
    lightFloodfill(x, y + 1, z, lightLevel);
    lightFloodfill(x, y - 1, z, lightLevel);
    lightFloodfill(x, y, z + 1, lightLevel);
    lightFloodfill(x, y, z - 1, lightLevel);
}

void World::lightFloodfill(int startX, int startY, int startZ, int startLightLevel)
{
    if (startLightLevel <= 0)
        return;

    std::queue<std::tuple<int, int, int, int>> queue; // (x, y, z, lightLevel)
    std::unordered_set<uint64_t> visited;

    auto encodePos = [](int x, int y, int z) -> uint64_t
    {
        return (static_cast<uint64_t>(x) & 0x3FFFFF) << 42 |
               (static_cast<uint64_t>(y) & 0x3FFFFF) << 21 |
               (static_cast<uint64_t>(z) & 0x1FFFFF);
    };

    queue.emplace(startX, startY, startZ, startLightLevel);

    while (!queue.empty())
    {
        auto [x, y, z, lightLevel] = queue.front();
        queue.pop();

        if (lightLevel <= 0)
            continue;

        // Position déjà visitée à une intensité supérieure ou égale
        uint64_t key = encodePos(x, y, z);
        if (!visited.insert(key).second)
            continue;

        {
            std::lock_guard<std::recursive_mutex> lock(worldMutex);

            std::shared_ptr<VoxelChunk> chunk = getChunkContaining(x, y, z);
            if (chunk == nullptr)
                continue;

            int localX = betterModulo(x, CHUNK_SIZE);
            int localY = betterModulo(y, CHUNK_SIZE);
            int localZ = betterModulo(z, CHUNK_SIZE);

            int bloc = chunk->getBloc(localX, localY, localZ);

            if (BlocDatabase::getInstance().isOpaque(bloc))
                continue;

            int currentLight = chunk->m_lights[localX][localY][localZ];
            // std::cout << "Light floodfill at " << x << ", " << y << ", " << z << " with level " << lightLevel << " Current is " << currentLight << std::endl;
            if (currentLight > lightLevel)
                continue;
            if (currentLight < lightLevel)
                chunk->dirty = true;

            chunk->m_lights[localX][localY][localZ] = lightLevel;
        }

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
    std::lock_guard<std::recursive_mutex> lock(worldMutex);
    std::shared_ptr<VoxelChunk> chunk = getChunkContaining(x, y, z);
    if (chunk == nullptr)
        return;

    chunk->m_lights[betterModulo(x, CHUNK_SIZE)][betterModulo(y, CHUNK_SIZE)][betterModulo(z, CHUNK_SIZE)] = lightLevel;
}

// Récupération de la position du personnage
void World::resolveCollisions(Character &character, World *world)
{
    if (character.getGamemode() == GAMEMODE_SPECTATOR)
    {
        character.move(character.vecteurDirection);
        return;
    }

    glm::vec3 minBB = character.getMinBoundingBox();
    glm::vec3 maxBB = character.getMaxBoundingBox();

    // Parcours des blocs proches de la bounding box du personnage
    for (int x = static_cast<int>(std::floor(minBB.x)); x <= static_cast<int>(std::floor(maxBB.x)); ++x)
    {
        for (int y = static_cast<int>(std::floor(minBB.y)); y <= static_cast<int>(std::floor(maxBB.y)); ++y)
        {
            for (int z = static_cast<int>(std::floor(minBB.z)); z <= static_cast<int>(std::floor(maxBB.z)); ++z)
            {
                int blockType = world->getBloc(x, y, z);

                // Ignore les blocs d'air et d'eau
                if (blockType == AIR || blockType == WATER)
                {
                    continue;
                }

                // Résolution des collisions pour les blocs solides
                resolveCollisionForBlock(character, glm::vec3(x, y, z));
            }
        }
    }

    // Application du mouvement du personnage
    character.move(character.vecteurDirection);
}

void World::resolveCollisions(Zombie &zombie, World *world)
{
    // Logique de collision similaire à celle utilisée pour Character
    // Récupérer la boîte englobante
    glm::vec3 minBB = zombie.getMinBoundingBox();
    glm::vec3 maxBB = zombie.getMaxBoundingBox();

    // Récupérer la direction de déplacement
    glm::vec3 &direction = zombie.vecteurDirection;

    // Traiter les collisions avec les blocs dans la boîte englobante
    for (int x = static_cast<int>(std::floor(minBB.x)); x <= static_cast<int>(std::floor(maxBB.x)); ++x)
    {
        for (int y = static_cast<int>(std::floor(minBB.y)); y <= static_cast<int>(std::floor(maxBB.y)); ++y)
        {
            for (int z = static_cast<int>(std::floor(minBB.z)); z <= static_cast<int>(std::floor(maxBB.z)); ++z)
            {
                int blockType = world->getBloc(x, y, z);

                // Ignorer l'air et l'eau
                if (blockType == AIR || blockType == WATER)
                {
                    continue;
                }

                // Résoudre les collisions avec les blocs solides
                resolveCollisionForBlock(zombie, glm::vec3(x, y, z));
            }
        }
    }

    // Appliquer le mouvement
    zombie.move(zombie.vecteurDirection);
}

void World::resolveCollisionForBlock(Zombie &zombie, glm::vec3 blockPosition)
{
    // Bounding box du zombie
    glm::vec3 minBB = zombie.getMinBoundingBox();
    glm::vec3 maxBB = zombie.getMaxBoundingBox();

    // Position minimale et maximale du bloc intersecté
    glm::vec3 blockMin = blockPosition;
    glm::vec3 blockMax = blockPosition + glm::vec3(1.0f);

    // Vérification si la bounding box du zombie intersecte celle du bloc
    if (maxBB.x > blockMin.x && minBB.x < blockMax.x &&
        maxBB.y > blockMin.y && minBB.y < blockMax.y &&
        maxBB.z > blockMin.z && minBB.z < blockMax.z)
    {

        // Calcul des overlaps sur chaque axe
        float overlapX = std::min(maxBB.x - blockMin.x, blockMax.x - minBB.x);
        float overlapY = std::min(maxBB.y - blockMin.y, blockMax.y - minBB.y);
        float overlapZ = std::min(maxBB.z - blockMin.z, blockMax.z - minBB.z);

        // Détermination de l'axe avec la plus petite profondeur de collision
        if (overlapX < overlapY && overlapX < overlapZ)
        { // Axe X
            if (zombie.vecteurDirection.x > 0 && zombie.getWorldPosition().x < blockPosition.x)
            {
                zombie.vecteurDirection.x = 0; // Bloque le mouvement vers la droite
            }
            else if (zombie.vecteurDirection.x < 0 && zombie.getWorldPosition().x > blockPosition.x)
            {
                zombie.vecteurDirection.x = 0; // Bloque le mouvement vers la gauche
            }
        }
        else if (overlapY < overlapX && overlapY < overlapZ)
        { // Axe Y
            if (zombie.vecteurDirection.y > 0 && zombie.getWorldPosition().y < blockPosition.y)
            {
                zombie.vecteurDirection.y = 0; // Bloque le mouvement vers le haut
            }
            else if (zombie.vecteurDirection.y < 0 && zombie.getWorldPosition().y > blockPosition.y)
            {
                zombie.vecteurDirection.y = 0; // Bloque le mouvement vers le bas
            }
        }
        else
        { // Axe Z
            if (zombie.vecteurDirection.z > 0 && zombie.getWorldPosition().z < blockPosition.z)
            {
                zombie.vecteurDirection.z = 0; // Bloque le mouvement vers l'avant
            }
            else if (zombie.vecteurDirection.z < 0 && zombie.getWorldPosition().z > blockPosition.z)
            {
                zombie.vecteurDirection.z = 0; // Bloque le mouvement vers l'arrière
            }
        }
    }
}

void World::addChunkColumn(std::shared_ptr<ChunkColumn> column)
{
    if (!column)
    {
        std::cerr << "Error: Attempted to add a null ChunkColumn to the world." << std::endl;
        return;
    }

    chunkColumns.emplace(column->getChunkCoords(), column);
}

void World::spawnEntities()
{
    //zombies
    m_mobSpawner->spawnZombiesInLoadedChunks();
}

void World::spawnTNT(glm::vec3 pos, glm::vec3 vel, GLuint programID)
{
    TNTProjectile* tnt = new TNTProjectile(pos, vel, 0.9f, this, programID);
    projectiles.push_back(tnt);
    getParent()->addChild(tnt);
}

void World::spawnEnderPearl(Character* character, glm::vec3 vel, GLuint programID)
{
    Projectile *projectile = new EnderPearl(character, vel, 0.9f, this, programID);
    projectiles.push_back(projectile);
    getParent()->addChild(projectile);
}

void World::removeTNT(Projectile* projectile, float explosionRadius)
{
    int count = 0;

    auto projectileToRemove = std::find(projectiles.begin(), projectiles.end(), projectile);
    
    // Apply explosion forces to all other projectiles in range
    for (auto it = projectiles.begin(); it != projectiles.end(); ++it)
    {
        if (*it == projectile)
            continue;
            
        glm::vec3 otherPos = (*it)->getWorldPosition();
        float distance = glm::distance(otherPos, projectile->getWorldPosition());
        if (distance < explosionRadius)
        {
            count++;
            glm::vec3 otherVel = (*it)->getVelocity();
            glm::vec3 direction = glm::normalize(otherPos - projectile->getWorldPosition());
            float force = distance / explosionRadius * EXPLOSION_ENTITY_PUSH_STRENGHT;
            (*it)->setVelocity(glm::vec3(
                otherVel.x + direction.x * force, 
                otherVel.y + direction.y * force, 
                otherVel.z + direction.z * force
            ));
        }
    }
    // Remove the exploding projectile
    if (projectileToRemove != projectiles.end())
    {
        getParent()->removeChild(*projectileToRemove);
        projectiles.erase(projectileToRemove);
    }
}

void World::removeProjectile(Projectile* projectile)
{
    auto it = std::find(projectiles.begin(), projectiles.end(), projectile);
    if (it != projectiles.end())
    {
        getParent()->removeChild(projectile);
        projectiles.erase(it);
    }
}

void World::updateEntities(float &deltaTime)
{
  
    for (auto &entity : entities)
    {
        entity->update(deltaTime);
    }
}

void World::renderEntities(GLuint &programID)
{
    for (auto &entity : entities)
    {
        entity->draw(programID);
    }
}
void World::resolveEntityGravity(float &deltaTime)
{
    for (auto &entity : entities)
    {
        entity->resolveGravity(deltaTime);
    }
}
void World::resolveEntityCollisions(float &deltaTime)
{
    for (auto &entity : entities)
    {
        // entity->resolveCollisions(deltaTime); //TODO A FAIRE + TARD car code horrible, met cette fonction dans le zombie dans Zombie et non dans world
        if (auto zombie = dynamic_cast<Zombie *>(entity.get()))
        {
            resolveCollisions(*zombie, this);
        }
    }
}
