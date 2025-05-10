#pragma once

#include <TP/Scene/VoxelChunk.hpp>
#include "common/fastNoiseLit.h"
#include <random>
#include <TP/Scene/World.hpp>
#include <TP/Scene/Biomes.hpp>

#define CAVE_BASE_THRESHOLD -10.1f
#define CAVE_DEPTH_SCALING_FACTOR 0.015f

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
    std::uniform_int_distribution<int> bedrockRng;

    int groundLevel = GROUND_LEVEL;

    int seed = GENERATION_SEED;

    BiomeManager biomeManager = BiomeManager(groundLevel, seed);

    void setBaseStone(std::shared_ptr<VoxelChunk> , int x, int z, const glm::ivec3 &worldAABBMin, int baseHeight);
    void generateTerrain(std::shared_ptr<VoxelChunk> , int i, int j, int k, int groundLevel);
    void decorateTerrain(std::shared_ptr<VoxelChunk> , int i, int j, int k, int groundLevel);

public:
    WorldGenerator();

    void genereteProceduralChunk(std::shared_ptr<VoxelChunk> , int i, int j, int k);
    void decorateProceduralChunk(std::shared_ptr<VoxelChunk> , int i, int j, int k);
};