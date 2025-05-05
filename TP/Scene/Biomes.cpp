#include <TP/Scene/Biomes.hpp>
#include <TP/Scene/BlocTypes.hpp>
#include <algorithm>
#include "Biomes.hpp"
#include <random>

/// ======================= ///
/// ===== PlainsBiome ===== ///
/// ======================= ///

float PlainsBiome::calculateHeight(float x, float z)
{
    return getGroundLevel() + std::max(getNoise()->GetNoise(x,z) * 5.0f + 1.f, 0.f);
}

void PlainsBiome::applySurface(VoxelChunk *chunk, int x, int z, int baseHeight, glm::ivec3 worldAABBMin)
{
    chunk->generationSetBloc(x, baseHeight - 3 - worldAABBMin.y, z, DIRT);
    chunk->generationSetBloc(x, baseHeight - 2 - worldAABBMin.y, z, DIRT);
    chunk->generationSetBloc(x, baseHeight - 1 - worldAABBMin.y, z, GRASS);
}

void PlainsBiome::decorate(VoxelChunk *chunk, int x, int z, int baseHeight)
{
    if (getRandom1000() < 10) { // 1% chance
        int b = chunk->getBloc(x, baseHeight - 1, z);
        if (b != GRASS) {
            return;
        }
        chunk->generationSetBloc(x, baseHeight - 1, z, DIRT);
        for (int y = 0; y < 3; y++) {
            chunk->generationSetBloc(x, baseHeight + y, z, LOG_OAK);
        }
        for (int dx = -2; dx <= 2; dx++) {
            for (int dz = -2; dz <= 2; dz++) {
                if (std::abs(dx) + std::abs(dz) <= 3) { // Simple circular leaf pattern
                    chunk->generationSetBloc(x + dx, baseHeight + 3, z + dz, LEAVES_OAK);
                }
                if (std::abs(dx) + std::abs(dz) <= 2) { // Smaller circular leaf pattern for upper layer
                    chunk->generationSetBloc(x + dx, baseHeight + 3 + 1, z + dz, LEAVES_OAK);
                }
                if (std::abs(dx) + std::abs(dz) <= 1) { // Smallest circular leaf pattern for top layer
                    chunk->generationSetBloc(x + dx, baseHeight + 3 + 2, z + dz, LEAVES_OAK);
                }
            }
        }
    }
}

/// ========================= ///
/// ===== MoutainsBiome ===== ///
/// ========================= ///

float MoutainsBiome::calculateHeight(float x, float z)
{
    float noiseValue = getNoise()->GetNoise(x,z);
    return getGroundLevel() + std::max(noiseValue * noiseValue * 80.0f + 3.f, 0.f);
}

void MoutainsBiome::applySurface(VoxelChunk *chunk, int x, int z, int baseHeight, glm::ivec3 worldAABBMin)
{
    int snowheight = 15 + snowNoise->GetNoise((float) x + worldAABBMin.x,(float) z + worldAABBMin.z) * 8;
    if (baseHeight > getGroundLevel() + snowheight) {
        for (int y = 0; y < 3; y++) {
            chunk->generationSetBloc(x, baseHeight - y - 2 - worldAABBMin.y, z, SNOW);
        }
    } else {
        chunk->generationSetBloc(x, baseHeight - 3 - worldAABBMin.y, z, STONE);
        chunk->generationSetBloc(x, baseHeight - 2 - worldAABBMin.y, z, STONE);
        chunk->generationSetBloc(x, baseHeight - 1 - worldAABBMin.y, z, STONE);
    }
}

void MoutainsBiome::decorate(VoxelChunk *chunk, int x, int z, int baseHeight)
{
}

int MoutainsBiome::getSnowHeight(float baseNoise)
{
    return 15 + baseNoise * 8;
}

/// ======================= ///
/// ===== DesertBiome ===== ///
/// ======================= ///

float DesertBiome::calculateHeight(float x, float z)
{
    float noiseValue = getNoise()->GetNoise(x,z);
    return getGroundLevel() + std::max(noiseValue, 0.f);
}

void DesertBiome::applySurface(VoxelChunk *chunk, int x, int z, int baseHeight, glm::ivec3 worldAABBMin)
{
    chunk->generationSetBloc(x, baseHeight - 6 - worldAABBMin.y, z, SANDSTONE);
    chunk->generationSetBloc(x, baseHeight - 5 - worldAABBMin.y, z, SANDSTONE);
    chunk->generationSetBloc(x, baseHeight - 4 - worldAABBMin.y, z, SANDSTONE);
    chunk->generationSetBloc(x, baseHeight - 3 - worldAABBMin.y, z, SAND);
    chunk->generationSetBloc(x, baseHeight - 2 - worldAABBMin.y, z, SAND);
    chunk->generationSetBloc(x, baseHeight - 1 - worldAABBMin.y, z, SAND);
}

void DesertBiome::decorate(VoxelChunk *chunk, int x, int z, int baseHeight)
{
    if (getRandom1000() < 1) {
        if (chunk->getBloc(x, baseHeight - 1, z) == AIR) {
            return;
        }
        int height = 3 + (getRandom1000() % 4); // Random height between 3 and 5
        for (int y = 0; y < height; y++) {
            chunk->generationSetBloc(x, baseHeight + y, z, IRON_BLOCK);
            //std::cout << "Iron rod bloc result : " << chunk->generationSetBloc(x, baseHeight + y, z, IRON_BLOCK) << std::endl;
        }
        for (int dx = -2; dx <= 2; dx++) {
            for (int dz = -2; dz <= 2; dz++) {
                if (std::abs(dx) + std::abs(dz) <= 1) { // Smallest circular leaf pattern for top layer
                    chunk->generationSetBloc(x + dx, baseHeight, z + dz, IRON_BLOCK);
                    chunk->generationSetBloc(x + dx, baseHeight-1, z + dz, IRON_BLOCK);
                }
            }
        }
    }
}

/// ========================= ///
/// ===== WaterBiome ======== ///
/// ========================= ///

float WaterBiome::calculateHeight(float x, float z)
{
    return getGroundLevel() + std::min(getNoise()->GetNoise(x,z) * -10.0f - 35.f, 0.f);
}


void WaterBiome::applySurface(VoxelChunk *chunk, int x, int z, int baseHeight, glm::ivec3 worldAABBMin)
{
    chunk->generationSetBloc(x, baseHeight - 3 - worldAABBMin.y, z, SAND);
    chunk->generationSetBloc(x, baseHeight - 2 - worldAABBMin.y, z, SAND);
    chunk->generationSetBloc(x, baseHeight - 1 - worldAABBMin.y, z, SAND);

    for (int y = 0; y < 16; y++) {
        if (y + worldAABBMin.y < getGroundLevel()-WATER_LEVEL_DELTA && y + worldAABBMin.y >= baseHeight && chunk->getBloc(x, y, z) == AIR) {
            chunk->generationSetBloc(x, y, z, WATER);
        }
    }
}

void WaterBiome::decorate(VoxelChunk *chunk, int x, int z, int baseHeight)
{
    // No decoration for water biome
}

/// ==================== ///
/// ===== IceBiome ===== ///
/// ==================== ///

float IceBiome::calculateHeight(float x, float z)
{
    return getGroundLevel() + std::max(getNoise()->GetNoise(x,z) * 5.0f + 1.f, 0.f);
}

void IceBiome::applySurface(VoxelChunk *chunk, int x, int z, int baseHeight, glm::ivec3 worldAABBMin)
{
    chunk->generationSetBloc(x, baseHeight - 5 - worldAABBMin.y, z, DIRT);
    chunk->generationSetBloc(x, baseHeight - 4 - worldAABBMin.y, z, DIRT);
    chunk->generationSetBloc(x, baseHeight - 3 - worldAABBMin.y, z, SNOW);
    chunk->generationSetBloc(x, baseHeight - 2 - worldAABBMin.y, z, SNOW);
    chunk->generationSetBloc(x, baseHeight - 1 - worldAABBMin.y, z, SNOW);
}

void IceBiome::decorate(VoxelChunk *chunk, int x, int z, int baseHeight)
{
}

/// ==================== ///
/// ===== CristalPeaksBiome ===== ///
/// ==================== ///

float CristalPeaksBiome::calculateHeight(float x, float z)
{
    float noiseValue = getNoise()->GetNoise(x,z);
    return getGroundLevel() + std::max(noiseValue * noiseValue * 70.0f + 3.f, 0.f);
}

void CristalPeaksBiome::applySurface(VoxelChunk *chunk, int x, int z, int baseHeight, glm::ivec3 worldAABBMin)
{
    chunk->generationSetBloc(x, baseHeight - 3 - worldAABBMin.y, z, SMOOTH_BASALT);
    chunk->generationSetBloc(x, baseHeight - 2 - worldAABBMin.y, z, SMOOTH_BASALT);
    chunk->generationSetBloc(x, baseHeight - 1 - worldAABBMin.y, z, SMOOTH_BASALT);
}

void CristalPeaksBiome::decorate(VoxelChunk *chunk, int x, int z, int baseHeight)
{
}

/// ======================== ///
/// ===== BiomeManager ===== ///
/// ======================== ///

BiomeManager::BiomeManager(int groundLevel, int seed) : groundLevel(groundLevel), seed(seed)
{
   
}

/**
 * @brief Add a biome to the manager and it's cellular noise. They should be set to CellularReturnType_Distance for nice blending.
 * 
 * @param biome 
 * @param noise 
 */
void BiomeManager::addBiome(std::unique_ptr<Biome> biome, float cellularNoiseFrequency) {
    biomes.push_back(std::move(biome));
    FastNoiseLite noise;
    noises.push_back(noise);
    noises.back().SetNoiseType(FastNoiseLite::NoiseType_Cellular);
    noises.back().SetFrequency(cellularNoiseFrequency);
    noises.back().SetCellularReturnType(FastNoiseLite::CellularReturnType_Distance);
    noises.back().SetCellularDistanceFunction(FastNoiseLite::CellularDistanceFunction_Euclidean);
    noises.back().SetCellularJitter(0.9f);
    noises.back().SetSeed(seed);
}

/**
 * @brief Get the weights of the biomes at the given coordinates.
 * 
 * @param x 
 * @param z 
 * @return std::vector<float> 
 */
std::vector<float> BiomeManager::getBiomeWeights(int x, int z)
{
    std::vector<float> weights;
    for (int i = 0; i < biomes.size(); ++i) {
        weights.push_back(noises[i].GetNoise((float) x + 150 * i,(float)  z - 150 * i));
        weights[i] = 1.0f - std::abs(weights[i]);
        weights[i] = std::pow(weights[i], 2.0f);
    }

    float sum = 0.0f;
    for (float w : weights) sum += w;
    for (float &w : weights) w /= sum;
    return weights;
}

/**
 * @brief Get the dominant biome based on the weights.
 * 
 * @param weights computed using getBiomeWeights before !!!!
 * @return Biome* 
 */
Biome *BiomeManager::getDominantBiome(const std::vector<float> &weights)
{
    float maxWeight = weights[0];
    Biome* biome = biomes[0].get();
    
    for (int i = 1; i < weights.size(); ++i) {
        if (weights[i] > maxWeight) {
            maxWeight = weights[i];
            biome = biomes[i].get();
        }
    }
    return biome;
}

/**
 * @brief Blend the height of the biomes at the given coordinates and returns the result.
 * 
 * @param weights 
 * @param x 
 * @param z 
 * @param worldAABBMin 
 * @return float 
 */
float BiomeManager::blendHeight(const std::vector<float> &weights, int x, int z, glm::ivec3 worldAABBMin)
{
    std::vector<float> heights;
    for (int i = 0; i < biomes.size(); ++i) {
        heights.push_back(biomes[i]->calculateHeight(x + worldAABBMin.x, z + worldAABBMin.z));
    }
    int baseHeight = 0;
    for (int i = 0; i < biomes.size(); ++i) {
        baseHeight += heights[i] * weights[i];
    }
    return baseHeight;
}
