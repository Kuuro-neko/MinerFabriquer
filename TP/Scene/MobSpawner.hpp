#pragma once

#include <memory>
#include <vector>
#include <TP/Scene/VoxelChunk.hpp>
#include <TP/Entities/Zombie.hpp>
#include <TP/Scene/World.hpp>
#include <random>


class MobSpawner  // Class to spawn monsters in the world
{
public:
    MobSpawner(World *world)
        : m_world(world), zombie_SpawnProbability(
                              ZOMBIE_SPAWN_PROBABILITY) {}

    void spawnZombiesInLoadedChunks();

private:
    World *m_world;
    float zombie_SpawnProbability;

    bool isCaveBlock(std::shared_ptr<VoxelChunk> chunk, int x, int y, int z);
    void spawnZombieAt(glm::vec3 position);
};