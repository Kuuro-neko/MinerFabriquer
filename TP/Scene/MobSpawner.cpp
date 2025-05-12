#include "MobSpawner.hpp"

void MobSpawner::spawnZombiesInLoadedChunks()
{
    auto loadedChunks = m_world->getAllChunks();
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    glm::vec3 playerPosition = m_world->getCamera()->getPosition();
    glm::ivec3 playerChunkCoords = glm::ivec3(
        playerPosition.x / CHUNK_SIZE,
        playerPosition.y / CHUNK_SIZE,
        playerPosition.z / CHUNK_SIZE);

    for (auto &chunk : loadedChunks)
    {
        // Calculate the distance from the player to the chunk
        glm::ivec3 chunkCoords = chunk->m_chunkCoords;
        float distanceToPlayer = glm::distance(glm::vec3(chunkCoords), glm::vec3(playerChunkCoords));

        // Only consider chunks within a certain radius and randomly decide to spawn zombies
        if (distanceToPlayer <= SPAWN_RADIUS && dis(gen) < ZOMBIE_SPAWN_PROBABILITY)
        {
    
            for (int x = 0; x < CHUNK_SIZE && chunk->entitiesNumber < MAX_ENTITIES_PER_CHUNK; ++x)
            {
                for (int y = 0; y < CHUNK_SIZE && chunk->entitiesNumber < MAX_ENTITIES_PER_CHUNK; ++y)
                {
                    for (int z = 0; z < CHUNK_SIZE && chunk->entitiesNumber < MAX_ENTITIES_PER_CHUNK; ++z)
                    {
                        if (isCaveBlock(chunk, x, y, z) && dis(gen) < zombie_SpawnProbability && chunk->entitiesNumber < MAX_ENTITIES_PER_CHUNK)
                        {
                            glm::vec3 worldPos = glm::vec3(
                                chunkCoords.x * CHUNK_SIZE + x,
                                chunkCoords.y * CHUNK_SIZE + y,
                                chunkCoords.z * CHUNK_SIZE + z);
                            spawnZombieAt(worldPos);
                            chunk->entitiesNumber++;
                        }
                    }
                }
            }
        }
    }
}

bool MobSpawner::isCaveBlock(std::shared_ptr<VoxelChunk> chunk, int x, int y, int z)
{
    int lightLevel = chunk->getLightLevel(x, y, z);
    int blockType = chunk->getBloc(x, y, z);
    return lightLevel == 0 && blockType == AIR && y < GROUND_LEVEL;
}

void MobSpawner::spawnZombieAt(glm::vec3 position)
{
    Zombie *zombie = new Zombie(
        Transform(position, DEFAULT_ROTATION, DEFAULT_SCALE),
        m_world,
        m_world->getCamera());
    zombie->setTexture(TextureManager::getInstance().getPBRTexture("zombie"));
    m_world->getParent()->addChild(zombie);
    m_world->addEntity(std::shared_ptr<Entity>(zombie));
    
    //std::cout << "Zombie spawned at " << position.x << ", " << position.y << ", " << position.z << std::endl;
}