#pragma once

#include <TP/Scene/VoxelChunk.hpp>
#include "common/fastNoiseLit.h"
#include <random>
#include <TP/Scene/World.hpp>

#define PLAINS 0
#define DESERT 1
#define MOUNTAINS 2

class WorldGenerator {
private:
    FastNoiseLite baseHeightNoise;
    FastNoiseLite mountainHeightNoise;
    FastNoiseLite caveNoise;
    FastNoiseLite caveNoise2;
    FastNoiseLite oreNoise;

    FastNoiseLite plainsNoise;
    FastNoiseLite moutainNoise;
    FastNoiseLite desertNoise;

    std::mt19937 rng;
    std::uniform_int_distribution<int> treeChance;
    std::uniform_int_distribution<int> ironRodChance;

    int groundLevel = 40;

    int seed = 777287;

    void generateTerrain(VoxelChunk *chunk, int i, int j, int k, int groundLevel);
    void addTrees(VoxelChunk *chunk, int x, int z, int baseHeight);
    void addIronRods(VoxelChunk *chunk, int x, int z, int baseHeight);
    std::vector<float> getBiomeWeights(int x, int z);
    int getBiome(std::vector<float> weights);

public:
    WorldGenerator();

    void genereteProceduralChunk(VoxelChunk *world, int i, int j, int k);
};