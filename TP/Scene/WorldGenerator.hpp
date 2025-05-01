#pragma once

#include <TP/Scene/VoxelChunk.hpp>
#include "common/fastNoiseLit.h"
#include <random>
#include <TP/Scene/World.hpp>
#include <TP/Scene/Biomes.hpp>

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

    int seed = 7772870;

    BiomeManager biomeManager = BiomeManager(groundLevel);

    void generateTerrain(VoxelChunk *chunk, int i, int j, int k, int groundLevel);
    void addTrees(VoxelChunk *chunk, int x, int z, int baseHeight);
    void addIronRods(VoxelChunk *chunk, int x, int z, int baseHeight);

public:
    WorldGenerator();

    void genereteProceduralChunk(VoxelChunk *world, int i, int j, int k);
};