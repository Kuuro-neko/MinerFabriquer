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
    FastNoiseLite baseNoise;
    FastNoiseLite mountainNoise;
    FastNoiseLite caveNoise;
    FastNoiseLite caveNoise2;
    FastNoiseLite oreNoise;
    FastNoiseLite biomeNoise;
    FastNoiseLite biomeNoise2;
    std::mt19937 rng;
    std::uniform_int_distribution<int> treeChance;

    int groundLevel = 40;

    void generateTerrain(VoxelChunk *chunk, int i, int j, int k, int groundLevel);
    void addTrees(VoxelChunk *chunk, int x, int z, int baseHeight);
    void addIronRods(VoxelChunk *chunk, int x, int z, int baseHeight);
    int getBiome(int x, int z);

public:
    WorldGenerator();

    void genereteProceduralChunk(VoxelChunk *world, int i, int j, int k);
};