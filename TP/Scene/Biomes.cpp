#include <TP/Scene/Biomes.hpp>
#include <TP/Scene/BlocTypes.hpp>
#include <algorithm>
#include "Biomes.hpp"
#include <random>
#include <TP/Scene/World.hpp> // Include the header defining the World class

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
    if (chunk->getBloc(x, baseHeight - 1, z) != GRASS) return;

    if (getRandom1000() < 10) { // 1% chance
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
    if (chunk->getBloc(x, baseHeight - 1, z) == AIR) return;

    if (getRandom1000() < 1) {
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
    if (chunk->getBloc(x, baseHeight - 1, z) == AIR) return;

    float p = getRandomFloat();

    if(p <= 0.0045f)  {
        addIceSpike(chunk, x, z, baseHeight, getRandomFloat() * 8 + 4, 1); // Small spikes
    } else if (p <= 0.0055f) {
        addIceSpike(chunk, x, z, baseHeight, getRandomFloat() * 20 + 4, 1); // High spikes
    }
}

void IceBiome::addIceSpike(VoxelChunk *chunk, int x, int z, int baseHeight, int height, int radius)
{
    int y = -2;
    int rings = getRandom1000() % 4 + 3;
    for (; y < 0; y++) {
        for (int dx = -radius; dx <= radius; dx++) {
            for (int dz = -radius; dz <= radius; dz++) {
                if (std::abs(dx) + std::abs(dz) <= radius) chunk->setBloc(x + dx, baseHeight + y, z + dz, ICE); // Set the base of the spike
            }
        }
    }
    for (; y < height-1; y++) {
        if (y % rings == 0) {
            for (int dx = -radius; dx <= radius; dx++) {
                for (int dz = -radius; dz <= radius; dz++) {
                    int delta = std::abs(dx) + std::abs(dz);
                    if(getRandomFloat() <= 1.0f - float(delta)/float(5*radius)) chunk->setBloc(x + dx, baseHeight + y, z + dz, ICE); // Repetitive rings
                }
            }
        } else {
            for (int dx = -radius; dx <= radius; dx++) {
                for (int dz = -radius; dz <= radius; dz++) {
                    if (std::abs(dx) + std::abs(dz) <= radius) {
                        int delta = std::abs(dx) + std::abs(dz);
                        if(getRandomFloat() <= 1.0f - float(delta)/float(3*radius)) chunk->generationSetBloc(x + dx, baseHeight + y, z + dz, ICE); // Normal radius
                    }
                }
            }
        }
    }
    // Set the top of the spike
    chunk->generationSetBloc(x, baseHeight + y, z, ICE);
    chunk->generationSetBloc(x, baseHeight + y + 1, z, ICE);
}

/// ============================= ///
/// ===== CristalPeaksBiome ===== ///
/// ============================= ///

float CristalPeaksBiome::calculateHeight(float x, float z)
{
    float noiseValue = getNoise()->GetNoise(x,z);
    return getGroundLevel() + std::max(noiseValue * noiseValue * 70.0f + 3.f, 0.f);
}

void CristalPeaksBiome::applySurface(VoxelChunk *chunk, int x, int z, int baseHeight, glm::ivec3 worldAABBMin)
{
    int r = getRandom1000();
    if (r % 23 < 10) chunk->generationSetBloc(x, baseHeight - 5 - worldAABBMin.y, z, SMOOTH_BASALT);
    if (r % 50 < 25) chunk->generationSetBloc(x, baseHeight - 4 - worldAABBMin.y, z, SMOOTH_BASALT);
    chunk->generationSetBloc(x, baseHeight - 3 - worldAABBMin.y, z, SMOOTH_BASALT);
    chunk->generationSetBloc(x, baseHeight - 2 - worldAABBMin.y, z, SMOOTH_BASALT);
    chunk->generationSetBloc(x, baseHeight - 1 - worldAABBMin.y, z, SMOOTH_BASALT);
}

void CristalPeaksBiome::decorate(VoxelChunk *chunk, int x, int z, int baseHeight)
{
    int wX = chunk->m_chunkCoords.x * CHUNK_SIZE + x;
    int wZ = chunk->m_chunkCoords.z * CHUNK_SIZE + z;
    for (float y = getGroundLevel() - 15.0f; y < baseHeight+2; y++) {
        if (y <= getGroundLevel()) {
            if (getRandomFloat() > (y + 16.0f - float(getGroundLevel())) /20.f) {
                continue;   
            }
        }
        float noiseValue = amethystNoise.GetNoise(x-136.0f,y,z-136.0f);
        if (noiseValue < 0.1f) {
            chunk->m_world->generationSetBloc(wX, y, wZ, CALCITE);
        } else if (noiseValue < 0.15f) {
            chunk->m_world->generationSetBloc(wX, y, wZ, AMETHYST);
        }
    }
}

/// ======================= ///
/// ===== MushroomBiome ===== ///
/// ======================= ///

float MushroomBiome::calculateHeight(float x, float z)
{
    float noiseValue = getNoise()->GetNoise(x,z);
    return getGroundLevel() + std::max(noiseValue, 0.f);
}

void MushroomBiome::applySurface(VoxelChunk *chunk, int x, int z, int baseHeight, glm::ivec3 worldAABBMin)
{
    chunk->generationSetBloc(x, baseHeight - 4 - worldAABBMin.y, z, DIRT);
    chunk->generationSetBloc(x, baseHeight - 3 - worldAABBMin.y, z, DIRT);
    float noiseValue = coralVineNoise.GetNoise((float) x,(float) z);
    if(noiseValue < -0.4f) {
        chunk->generationSetBloc(x, baseHeight - 2 - worldAABBMin.y, z, MOSS);
        if (noiseValue > -0.44f) {
            chunk->generationSetBloc(x, baseHeight - 1 - worldAABBMin.y, z, GLOWSTONE);
        } else {
            chunk->generationSetBloc(x, baseHeight - 1 - worldAABBMin.y, z, MOSS);
        }
    } else {
        chunk->generationSetBloc(x, baseHeight - 2 - worldAABBMin.y, z, DIRT);
        chunk->generationSetBloc(x, baseHeight - 1 - worldAABBMin.y, z, MYCELIUM);
    }
}

void MushroomBiome::decorate(VoxelChunk *chunk, int x, int z, int baseHeight)
{
    if (chunk->getBloc(x, baseHeight - 1, z) == AIR) return;

    if (getRandomFloat() >= 0.9975f) {
        int stemRadius = getRandom1000() % 2;
        float capRadius = stemRadius == 0 ? 1.0f + getRandomFloat() * 2.0f : 3.0f + getRandomFloat() * 2.0f;
        addMushroomCup(chunk, x, z, baseHeight, getRandomFloat() * 5.0f + 3, stemRadius, capRadius, MUSHROOM_STEM, BROWN_MUSHROOM);
    } else if (getRandomFloat() >= 0.9955f) {
        int h = getRandom1000() % 2 + 5;
        float r =  1.5f + getRandomFloat() * 2.0f;
        addMushroomReverseU(chunk, x, z, baseHeight, h, r, MUSHROOM_STEM, RED_MUSHROOM);
    }
}

void MushroomBiome::addMushroomCup(VoxelChunk *chunk, int x, int z, int baseHeight, int stemHeight, int stemRadius, float capRadius, int stemMaterial, int capMaterial)
{
    for (int dx = -stemRadius-1; dx <= stemRadius; dx+=stemRadius+1) {
        for (int dz = -stemRadius-1; dz <= stemRadius; dz+=stemRadius+1) {
            if (chunk->getBloc(x + dx, baseHeight + 2, z + dz) != AIR) {
                return;
            }
        }
    }
    int distance;
    for (int dx = -stemRadius; dx <= stemRadius; dx++) {
        for (int dz = -stemRadius; dz <= stemRadius; dz++) {
            distance = std::abs(dx) + std::abs(dz);
            // Set the base of the mushroom
            for (int y = -2; y < 0; y++) {
                if (distance <= stemRadius) {
                    chunk->setBloc(x + dx, baseHeight + y, z + dz, stemMaterial);
                }
            }
            // Set the stem of the mushroom
            for (int y = 0; y < stemHeight-1; y++) {
                
                if (distance <= stemRadius) {
                    chunk->generationSetBloc(x + dx, baseHeight + y, z + dz, stemMaterial);
                }
            }
        }
    }
    // Set the cap of the mushroom
    int y = stemHeight-1;
    for (int dx = -capRadius; dx <= capRadius; dx++) {
        for (int dz = -capRadius; dz <= capRadius; dz++) {
            distance = std::sqrt(dx * dx + dz * dz);
            if (distance < capRadius-2.0f) {
                chunk->generationSetBloc(x + dx, baseHeight + y + 1, z + dz, WATER);
                if(dx == 0 || dz == 0 ) {
                    chunk->generationSetBloc(x + dx, baseHeight + y, z + dz, stemMaterial);
                } else {
                    chunk->generationSetBloc(x + dx, baseHeight + y, z + dz, capMaterial);
                }
            } else if (distance <= capRadius-1.0f) {
                chunk->generationSetBloc(x + dx, baseHeight + y + 1, z + dz, capMaterial);
                if(dx == 0 || dz == 0) {
                    chunk->generationSetBloc(x + dx, baseHeight + y, z + dz, stemMaterial);
                }
            } else if (distance < capRadius) {
                if(dx == 0 || dz == 0) {
                    chunk->generationSetBloc(x + dx, baseHeight + y + 1, z + dz, stemMaterial);
                }
            }
        }
    }
    chunk->generationSetBloc(x, baseHeight + y, z, stemMaterial);
}

void MushroomBiome::addMushroomReverseU(VoxelChunk *chunk, int x, int z, int baseHeight, int stemHeight, float capRadius, int stemMaterial, int capMaterial)
{
    float distance;
    // Set the base of the mushroom
    for (int y = -2; y < 2; y++) {
        chunk->generationSetBloc(x, baseHeight + y, z, stemMaterial);
    }
    glm::ivec3 center = glm::ivec3(x, baseHeight + capRadius/2, z);
    for (int y = baseHeight + capRadius*3; y >= baseHeight + 2; y--) {
        for (int dx = -capRadius*2; dx <= capRadius*2; dx++) {
            for (int dz = -capRadius*2; dz <= capRadius*2; dz++) {
                // Set the cap
                distance = std::sqrt(std::pow(x + dx - center.x, 2) + std::pow(z + dz - center.z, 2) + std::pow(y - center.y, 2));
                if (distance >= capRadius - 0.3f && distance <= capRadius + 0.7f) {
                    chunk->generationSetBloc(x + dx, y, z + dz, capMaterial);
                }

            }
        }
        // Set the stem
        if (y <= capRadius - 0.3f) {
            chunk->generationSetBloc(center.x, y, center.z, stemMaterial);
        } else if (y <= capRadius + 0.7f) {
            chunk->generationSetBloc(center.x, y, center.z, capMaterial);
        }
    }
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
