#pragma once

#include <TP/Scene/VoxelChunk.hpp>
#include "common/fastNoiseLit.h"
#include <random>
#include <TP/Scene/World.hpp>

class WorldGenerator {
private:
    FastNoiseLite baseNoise;
    FastNoiseLite mountainNoise;
    std::mt19937 rng;
    std::uniform_int_distribution<int> treeChance;

    int groundLevel = 4;

    void generateTerrain(VoxelChunk *chunk, int i, int j, int groundLevel);
    void addTrees(VoxelChunk *chunk, int x, int z, int baseHeight);

public:
    WorldGenerator();

    void genereteProceduralChunk(VoxelChunk *world, int i, int j);
};