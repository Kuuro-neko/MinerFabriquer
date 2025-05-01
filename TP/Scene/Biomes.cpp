#include <TP/Scene/Biomes.hpp>
#include <TP/Scene/BlocTypes.hpp>
#include <algorithm>
#include "Biomes.hpp"

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
}

/// ======================== ///
/// ===== BiomeManager ===== ///
/// ======================== ///

BiomeManager::BiomeManager(int groundLevel) : groundLevel(groundLevel)
{

}

/**
 * @brief Add a biome to the manager and it's cellular noise. They should be set to CellularReturnType_Distance for nice blending.
 * 
 * @param biome 
 * @param noise 
 */
void BiomeManager::addBiome(std::unique_ptr<Biome> biome, FastNoiseLite* noise) {
    biomes.push_back(std::move(biome));
    noises.push_back(noise);
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
    float nx = x * 0.01f;
    float nz = z * 0.011f;

    std::vector<float> weights;
    for (int i = 0; i < biomes.size(); ++i) {
        weights.push_back(noises[i]->GetNoise(nx + 150 * i, nz - 150 * i));
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
