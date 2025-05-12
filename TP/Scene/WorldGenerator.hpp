#pragma once

#include <TP/Scene/VoxelChunk.hpp>
#include "common/fastNoiseLit.h"
#include <random>
#include <TP/Scene/World.hpp>
#include <TP/Scene/Biomes.hpp>

#define CAVE_BASE_THRESHOLD -0.3f
#define CAVE_DEPTH_SCALING_FACTOR 0.018f

#define IRON_THRESHOLD 0.28f

class WorldGenerator {
private:
    FastNoiseLite caveNoise;
    FastNoiseLite caveNoise2;
    FastNoiseLite oreNoise;
    FastNoiseLite bedrockNoise;

    std::mt19937 rng;

    int groundLevel = GROUND_LEVEL;

    
    BiomeManager biomeManager = BiomeManager(groundLevel, seed);
    
    void setBaseStone(std::shared_ptr<VoxelChunk> , int x, int z, const glm::ivec3 &worldAABBMin, int baseHeight);
    void generateTerrain(std::shared_ptr<VoxelChunk> , int i, int j, int k, int groundLevel);
    void decorateTerrain(std::shared_ptr<VoxelChunk> , int i, int j, int k, int groundLevel);
    
    
    WorldGenerator();


    std::string seedStr = "default";
    int seed = stringToInt(seedStr);
public:

    static WorldGenerator &getInstance() {
        static WorldGenerator instance;
        return instance;
    }

    void setSeed(std::string seed);
    std::string getSeedStr();

    void genereteProceduralChunk(std::shared_ptr<VoxelChunk> , int i, int j, int k);
    void decorateProceduralChunk(std::shared_ptr<VoxelChunk> , int i, int j, int k);
};