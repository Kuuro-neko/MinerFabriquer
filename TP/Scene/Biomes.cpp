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

void DesertBiome::decorate(VoxelChunk *chunk, int x, int z, int baseHeight){

}

/// ========================= ///
/// ===== WaterBiome ===== ///
/// ========================= ///

float WaterBiome::calculateHeight(float x, float z)
{
    return -getGroundLevel();
}

//function that returns the minimum height of the chunk'neighbors
int WaterBiome::getMinNeighborHeight(VoxelChunk *chunk, int x, int z, glm::ivec3 worldAABBMin)
{
    int minHeight = INT_MAX;
    for (int dx = -1; dx <= 1; ++dx)
    {
        for (int dz = -1; dz <= 1; ++dz)
        {
            if (dx == 0 && dz == 0) continue; // Skip the current chunk

            int neighborX = x + dx;
            int neighborZ = z + dz;

            // Check if the neighbor is within the chunk bounds
            if (neighborX >= 0 && neighborX < chunk->m_sizeX && neighborZ >= 0 && neighborZ < chunk->m_sizeZ)
            {
                int neighborHeight = chunk->getBloc(neighborX, worldAABBMin.y, neighborZ);
                minHeight = std::min(minHeight, neighborHeight);
            }
        }
    }

    return minHeight;
}
//TODO pas de bedrock + pas lisse entre les biomes
void WaterBiome::applySurface(VoxelChunk *chunk, int x, int z, int baseHeight, glm::ivec3 worldAABBMin)
{
    const int WATER_LEVEL = 64; // Niveau d'eau fixe

    // Calculer la hauteur maximale de l'eau en fonction des voisins
    int minNeighborHeight = getMinNeighborHeight(chunk, x, z, worldAABBMin);
    int maxWaterHeight = std::min(WATER_LEVEL, minNeighborHeight);

    // Parcourir chaque bloc du chunk
    for (int localX = 0; localX < chunk->m_sizeX; ++localX)
    {
        for (int localZ = 0; localZ < chunk->m_sizeZ; ++localZ)
        {
            for (int localY = 0; localY < chunk->m_sizeY; ++localY)
            {
                int globalY = localY + worldAABBMin.y;

                if (globalY <= 0)
                {
                    // Placer la bedrock à la couche la plus basse
                    chunk->generationSetBloc(localX, localY, localZ, BEDROCK);
                }
                else if (globalY <= maxWaterHeight)
                {
                    // Remplir avec de l'eau jusqu'à la hauteur maximale
                    chunk->generationSetBloc(localX, localY, localZ, WATER);
                }
                else
                {
                    // Remplir au-dessus de la hauteur maximale avec de l'air
                    chunk->generationSetBloc(localX, localY, localZ, AIR);
                }
            }
        }
    }
}

void WaterBiome::decorate(VoxelChunk *chunk, int x, int z, int baseHeight)
{
    // No decoration for water biome
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
    noises.back().SetNoiseType(FastNoiseLite::NoiseType_Cellular);
    noises.back().SetFrequency(cellularNoiseFrequency);
    noises.back().SetCellularReturnType(FastNoiseLite::CellularReturnType_Distance);
    noises.back().SetCellularDistanceFunction(FastNoiseLite::CellularDistanceFunction_Manhattan);
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
    float nx = x * 0.01f;
    float nz = z * 0.011f;

    std::vector<float> weights;
    for (int i = 0; i < biomes.size(); ++i) {
        weights.push_back(noises[i].GetNoise(nx + 150 * i, nz - 150 * i));
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
