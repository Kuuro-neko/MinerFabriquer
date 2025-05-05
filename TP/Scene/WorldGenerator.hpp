#pragma once

#include <TP/Scene/VoxelChunk.hpp>
#include "common/fastNoiseLit.h"
#include <random>
#include <TP/Scene/World.hpp>
#include <TP/Scene/Biomes.hpp>

#define CAVE_BASE_THRESHOLD -0.75f
#define CAVE_DEPTH_SCALING_FACTOR 0.01f

#define IRON_THRESHOLD 0.28f

class WorldGenerator {
private:
    FastNoiseLite baseHeightNoise;
    FastNoiseLite mountainHeightNoise;
    FastNoiseLite caveNoise;
    FastNoiseLite caveNoise2;
    FastNoiseLite oreNoise;
    FastNoiseLite waterHolesHeightNoise;

    std::mt19937 rng;
    std::uniform_int_distribution<int> treeChance;
    std::uniform_int_distribution<int> ironRodChance;

    int groundLevel = GROUND_LEVEL;

    int seed = GENERATION_SEED;

    BiomeManager biomeManager = BiomeManager(groundLevel, seed);

    void setBaseStone(VoxelChunk *chunk, int x, int z, const glm::ivec3 &worldAABBMin, int baseHeight);
    void generateTerrain(World *world, VoxelChunk *chunk, int i, int j, int k, int groundLevel);
    void addTrees(VoxelChunk *chunk, int x, int z, int baseHeight);
    void addIronRods(VoxelChunk *chunk, int x, int z, int baseHeight);

public:
    WorldGenerator();

    void genereteProceduralChunk(World *world, VoxelChunk *chunk, int i, int j, int k);
};