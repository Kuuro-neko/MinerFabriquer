#include <TP/Scene/Biomes.hpp>
#include <TP/Scene/BlocTypes.hpp>
#include <algorithm>
#include "Biomes.hpp"
#include <random>
#include <TP/Scene/World.hpp> // Include the header defining the World class

/// ======================= ///
/// ===== PlainsBiome ===== ///
/// ======================= ///

void PlainsBiome::applySurface(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight, glm::ivec3 worldAABBMin)
{
    chunk->generationSetBloc(x, baseHeight - 3 - worldAABBMin.y, z, DIRT);
    chunk->generationSetBloc(x, baseHeight - 2 - worldAABBMin.y, z, DIRT);
    chunk->generationSetBloc(x, baseHeight - 1 - worldAABBMin.y, z, DIRT);
    chunk->generationSetBloc(x, baseHeight - worldAABBMin.y, z, GRASS);
}

void PlainsBiome::decorate(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight)
{
    // if (chunk->getBloc(x, baseHeight, z) != GRASS) return;
    if (chunk->getBloc(x, baseHeight+2, z) != AIR) return;

    if (getRandom1000() < 5) {
        chunk->generationSetBloc(x, baseHeight, z, DIRT);
        for (int y = 1; y < 4; y++) {
            chunk->generationSetBloc(x, baseHeight + y, z, LOG_OAK);
        }
        for (int dx = -2; dx <= 2; dx++) {
            for (int dz = -2; dz <= 2; dz++) {
                if (std::abs(dx) + std::abs(dz) <= 3) { // Simple circular leaf pattern
                    chunk->generationSetBloc(x + dx, baseHeight + 4, z + dz, LEAVES_OAK);
                }
                if (std::abs(dx) + std::abs(dz) <= 2) { // Smaller circular leaf pattern for upper layer
                    chunk->generationSetBloc(x + dx, baseHeight + 4 + 1, z + dz, LEAVES_OAK);
                }
                if (std::abs(dx) + std::abs(dz) <= 1) { // Smallest circular leaf pattern for top layer
                    chunk->generationSetBloc(x + dx, baseHeight + 4 + 2, z + dz, LEAVES_OAK);
                }
            }
        }
    }
}

/// ========================= ///
/// ===== MoutainsBiome ===== ///
/// ========================= ///

void MoutainsBiome::applySurface(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight, glm::ivec3 worldAABBMin)
{
    int snowheight = 22 + snowNoise.GetNoise((float) x + worldAABBMin.x,(float) z + worldAABBMin.z) * 8;
    if (baseHeight > getGroundLevel() + snowheight) {
        for (int y = 0; y < 3; y++) {
            chunk->generationSetBloc(x, baseHeight - y - worldAABBMin.y, z, SNOW);
        }
    } else {
        chunk->generationSetBloc(x, baseHeight - 3 - worldAABBMin.y, z, STONE);
        chunk->generationSetBloc(x, baseHeight - 2 - worldAABBMin.y, z, STONE);
        chunk->generationSetBloc(x, baseHeight - 1 - worldAABBMin.y, z, STONE);
        chunk->generationSetBloc(x, baseHeight - worldAABBMin.y, z, STONE);
    }
}

void MoutainsBiome::decorate(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight)
{
}

int MoutainsBiome::getSnowHeight(float baseNoise)
{
    return 15 + baseNoise * 8;
}

/// ======================= ///
/// ===== DesertBiome ===== ///
/// ======================= ///

void DesertBiome::applySurface(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight, glm::ivec3 worldAABBMin)
{
    chunk->generationSetBloc(x, baseHeight - 6 - worldAABBMin.y, z, SANDSTONE);
    chunk->generationSetBloc(x, baseHeight - 5 - worldAABBMin.y, z, SANDSTONE);
    chunk->generationSetBloc(x, baseHeight - 4 - worldAABBMin.y, z, SANDSTONE);
    chunk->generationSetBloc(x, baseHeight - 3 - worldAABBMin.y, z, SAND);
    chunk->generationSetBloc(x, baseHeight - 2 - worldAABBMin.y, z, SAND);
    chunk->generationSetBloc(x, baseHeight - 1 - worldAABBMin.y, z, SAND);
    chunk->generationSetBloc(x, baseHeight - worldAABBMin.y, z, SAND);
}

void DesertBiome::decorate(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight)
{

}

/// ========================= ///
/// ===== OceanBiome ======== ///
/// ========================= ///

void OceanBiome::applySurface(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight, glm::ivec3 worldAABBMin)
{
    chunk->generationSetBloc(x, baseHeight - 6 - worldAABBMin.y, z, SMOOTH_BASALT);
    chunk->generationSetBloc(x, baseHeight - 5 - worldAABBMin.y, z, SMOOTH_BASALT);
    chunk->generationSetBloc(x, baseHeight - 4 - worldAABBMin.y, z, SMOOTH_BASALT);
    chunk->generationSetBloc(x, baseHeight - 3 - worldAABBMin.y, z, SMOOTH_BASALT);
    chunk->generationSetBloc(x, baseHeight - 2 - worldAABBMin.y, z, SAND);
    chunk->generationSetBloc(x, baseHeight - 1 - worldAABBMin.y, z, SAND);
    chunk->generationSetBloc(x, baseHeight - worldAABBMin.y, z, SAND);
}

void OceanBiome::decorate(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight)
{
    //if (chunk->getBloc(x, baseHeight, z) != SAND) return;
    if (chunk->getBloc(x, baseHeight+3, z) != WATER) return;
    if (getRandomFloat() < 0.998f) return;

    if (coralNoise.GetNoise((float) x,(float) z) > 0.1f) {
        chunk->generationSetBloc(x, baseHeight, z, BRAIN_CORAL);
        chunk->generationSetBloc(x, baseHeight+1, z, BRAIN_CORAL);
        chunk->generationSetBloc(x, baseHeight+2, z, BRAIN_CORAL);
        for (int dx = -1; dx <= 1; dx++) {
            for (int dz = -1; dz <= 1; dz++) {
                if (dx == 0 || dz == 0) {
                    if (dx == dz) continue;
                    chunk->generationSetBloc(x + dx, baseHeight+3, z + dz, BRAIN_CORAL);
                    chunk->generationSetBloc(x + dx, baseHeight+4, z + dz, BRAIN_CORAL);
                    chunk->generationSetBloc(x + dx, baseHeight+5, z + dz, BRAIN_CORAL);
                }
            }
        }
    } else {
        chunk->generationSetBloc(x, baseHeight, z, BUBBLE_CORAL);
        chunk->generationSetBloc(x, baseHeight+1, z, BUBBLE_CORAL);
        chunk->generationSetBloc(x, baseHeight+2, z, BUBBLE_CORAL);
        for (int dx = -1; dx <= 1; dx+=2) {
            for (int dz = -1; dz <= 1; dz+=2) {
                chunk->generationSetBloc(x + dx, baseHeight+3, z + dz, BUBBLE_CORAL);
                chunk->generationSetBloc(x + dx, baseHeight+4, z + dz, BUBBLE_CORAL);
                chunk->generationSetBloc(x + dx, baseHeight+5, z + dz, BUBBLE_CORAL);
            }
        }
    }
}

/// ==================== ///
/// ===== IceBiome ===== ///
/// ==================== ///

void IceBiome::applySurface(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight, glm::ivec3 worldAABBMin)
{
    chunk->generationSetBloc(x, baseHeight - 5 - worldAABBMin.y, z, DIRT);
    chunk->generationSetBloc(x, baseHeight - 4 - worldAABBMin.y, z, DIRT);
    chunk->generationSetBloc(x, baseHeight - 3 - worldAABBMin.y, z, SNOW);
    chunk->generationSetBloc(x, baseHeight - 2 - worldAABBMin.y, z, SNOW);
    chunk->generationSetBloc(x, baseHeight - 1 - worldAABBMin.y, z, SNOW);
    chunk->generationSetBloc(x, baseHeight - worldAABBMin.y, z, SNOW);
}

void IceBiome::decorate(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight)
{
    //if (chunk->getBloc(x, baseHeight, z) == AIR) return;
    if (chunk->getBloc(x, baseHeight+2, z) != AIR) return;

    float p = getRandomFloat();

    if(p <= 0.0045f)  {
        addIceSpike(chunk, x, z, baseHeight, getRandomFloat() * 8 + 4, 1); // Small spikes
    } else if (p <= 0.0055f) {
        addIceSpike(chunk, x, z, baseHeight, getRandomFloat() * 20 + 4, 1); // High spikes
    }
}

void IceBiome::addIceSpike(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight, int height, int radius)
{
    int y = -2;
    int rings = getRandom1000() % 4 + 3;
    for (; y < 0; y++) {
        for (int dx = -radius; dx <= radius; dx++) {
            for (int dz = -radius; dz <= radius; dz++) {
                if (std::abs(dx) + std::abs(dz) <= radius) chunk->generationSetBloc(x + dx, baseHeight + y, z + dz, ICE); // Set the base of the spike
            }
        }
    }
    for (; y < height-1; y++) {
        if (y % rings == 0) {
            for (int dx = -radius; dx <= radius; dx++) {
                for (int dz = -radius; dz <= radius; dz++) {
                    int delta = std::abs(dx) + std::abs(dz);
                    if(getRandomFloat() <= 1.0f - float(delta)/float(5*radius)) chunk->generationSetBloc(x + dx, baseHeight + y, z + dz, ICE); // Repetitive rings
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

void CristalPeaksBiome::applySurface(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight, glm::ivec3 worldAABBMin)
{
    int r = getRandom1000();
    if (r % 23 < 10) chunk->generationSetBloc(x, baseHeight - 5 - worldAABBMin.y, z, SMOOTH_BASALT);
    if (r % 50 < 25) chunk->generationSetBloc(x, baseHeight - 4 - worldAABBMin.y, z, SMOOTH_BASALT);
    chunk->generationSetBloc(x, baseHeight - 3 - worldAABBMin.y, z, SMOOTH_BASALT);
    chunk->generationSetBloc(x, baseHeight - 2 - worldAABBMin.y, z, SMOOTH_BASALT);
    chunk->generationSetBloc(x, baseHeight - 1 - worldAABBMin.y, z, SMOOTH_BASALT);
    chunk->generationSetBloc(x, baseHeight - worldAABBMin.y, z, SMOOTH_BASALT);
}

void CristalPeaksBiome::decorate(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight)
{
    int wX = chunk->m_chunkCoords.x * CHUNK_SIZE + x;
    int wZ = chunk->m_chunkCoords.z * CHUNK_SIZE + z;
    for (float y = getGroundLevel() - 15.0f; y < baseHeight+2; y++) {
        if (y <= getGroundLevel()) {
            if (getRandomFloat() > (y + 16.0f - float(getGroundLevel())) /20.f) {
                continue;   
            }
        }
        float noiseValue = amethystNoise.GetNoise((float)x+chunk->m_chunkCoords.x * CHUNK_SIZE,(float) y, (float)z+chunk->m_chunkCoords.z * CHUNK_SIZE);
        if (noiseValue < 0.05f) {
            chunk->generationSetBloc(wX, y, wZ, AMETHYST);
        } else if (noiseValue < 0.09f) {
            chunk->generationSetBloc(wX, y, wZ, CALCITE);
        }
    }
}

/// ========================= ///
/// ===== MushroomBiome ===== ///
/// ========================= ///

void MushroomBiome::applySurface(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight, glm::ivec3 worldAABBMin)
{
    chunk->generationSetBloc(x, baseHeight - 4 - worldAABBMin.y, z, DIRT);
    chunk->generationSetBloc(x, baseHeight - 3 - worldAABBMin.y, z, DIRT);
    float noiseValue = coralVineNoise.GetNoise((float) x,(float) z);
    if(noiseValue < -0.4f) {
        chunk->generationSetBloc(x, baseHeight - 1 - worldAABBMin.y, z, DIRT);
        chunk->generationSetBloc(x, baseHeight - 1 - worldAABBMin.y, z, MOSS);
        if (noiseValue > -0.44f) {
            chunk->generationSetBloc(x, baseHeight - worldAABBMin.y, z, GLOWSTONE);
        } else {
            chunk->generationSetBloc(x, baseHeight - worldAABBMin.y, z, MOSS);
        }
    } else {
        chunk->generationSetBloc(x, baseHeight - 2 - worldAABBMin.y, z, DIRT);
        chunk->generationSetBloc(x, baseHeight - 1 - worldAABBMin.y, z, DIRT);
        chunk->generationSetBloc(x, baseHeight - worldAABBMin.y, z, MYCELIUM);
    }
}

void MushroomBiome::decorate(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight)
{
    if (chunk->getBloc(x, baseHeight, z, true, false) == AIR) return;
    if (chunk->getBloc(x, baseHeight + 2, z, true, false) != AIR) return;

    if (getRandomFloat() >= 0.9975f) {
        int stemRadius = getRandom1000() % 2;
        float capRadius = stemRadius == 0 ? 1.0f + getRandomFloat() * 2.0f : 3.0f + getRandomFloat() * 2.0f;
        addMushroomCup(chunk, x, z, baseHeight+1, getRandomFloat() * 5.0f + 3, stemRadius, capRadius, MUSHROOM_STEM, BROWN_MUSHROOM);
    } else if (getRandomFloat() >= 0.9955f) {
        int h = getRandom1000() % 2 + 5;
        float r =  1.5f + getRandomFloat() * 2.0f;
        addMushroomReverseU(chunk, x, z, baseHeight+1, h, r, MUSHROOM_STEM, RED_MUSHROOM);
    }
}

void MushroomBiome::addMushroomCup(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight, int stemHeight, int stemRadius, float capRadius, int stemMaterial, int capMaterial)
{
    int distance;
    for (int dx = -stemRadius; dx <= stemRadius; dx++) {
        for (int dz = -stemRadius; dz <= stemRadius; dz++) {
            distance = std::abs(dx) + std::abs(dz);
            // Set the base of the mushroom
            for (int y = -2; y < 0; y++) {
                if (distance <= stemRadius) {
                    chunk->generationSetBloc(x + dx, baseHeight + y, z + dz, stemMaterial);
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

void MushroomBiome::addMushroomReverseU(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight, int stemHeight, float capRadius, int stemMaterial, int capMaterial)
{
    float distance;
    // Set the base of the mushroom
    for (int y = -2; y < capRadius; y++) {
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
        if (y <= capRadius + 0.7f && y > capRadius - 0.3f) {
            chunk->generationSetBloc(center.x, y, center.z, capMaterial);
        }
    }
}

/// ====================== ///
/// ===== BeachBiome ===== ///
/// ====================== ///

void BeachBiome::applySurface(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight, glm::ivec3 worldAABBMin)
{
    chunk->generationSetBloc(x, baseHeight - 2 - worldAABBMin.y, z, SAND);
    chunk->generationSetBloc(x, baseHeight - 1 - worldAABBMin.y, z, SAND);
    chunk->generationSetBloc(x, baseHeight - worldAABBMin.y, z, SAND);
}

void BeachBiome::decorate(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight)
{
}

/// ============================ ///
/// ===== FrozenBeachBiome ===== ///
/// ============================ ///

void FrozenBeachBiome::applySurface(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight, glm::ivec3 worldAABBMin)
{
    int blocAbove = chunk->getBloc(x, baseHeight+1, z);
    int bloc = chunk->getBloc(x, baseHeight, z);
    //std::cout << "coord bloc above: " << x << " " << baseHeight + 1 << " " << z << std::endl;
    if (blocAbove == AIR) {
        chunk->generationSetBloc(x, baseHeight - worldAABBMin.y, z, SNOW);
        for (int y = 1; y < 3; y++) {
            chunk->generationSetBloc(x, baseHeight - y - worldAABBMin.y, z, SAND);
        }
    }
    if (blocAbove == WATER || bloc == WATER) {
        chunk->generationSetBloc(x, WATER_LEVEL - 1 - worldAABBMin.y, z, ICE);
        for (int y = 1; y < 3; y++) {
            chunk->generationSetBloc(x, baseHeight - y - worldAABBMin.y, z, SAND);
        }

    }
}

void FrozenBeachBiome::decorate(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight)
{
}

/// ============================ ///
/// ===== FrozenOceanBiome ===== ///
/// ============================ ///

void FrozenOceanBiome::applySurface(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight, glm::ivec3 worldAABBMin)
{
    int blocAbove = chunk->getBloc(x, baseHeight+1, z);
    int bloc = chunk->getBloc(x, baseHeight, z);
    //std::cout << "coord bloc above: " << x << " " << baseHeight + 1 << " " << z << std::endl;
    if (blocAbove == AIR) {
        chunk->generationSetBloc(x, baseHeight - worldAABBMin.y, z, SNOW);
        for (int y = 1; y < 3; y++) {
            chunk->generationSetBloc(x, baseHeight - y - worldAABBMin.y, z, SAND);
        }
    }
    if (blocAbove == WATER || bloc == WATER) {
        if (iceNoise.GetNoise((float) x + worldAABBMin.x,(float) z + worldAABBMin.z) > -0.4f) {
            chunk->generationSetBloc(x, WATER_LEVEL - 1 - worldAABBMin.y, z, ICE);
        }
        for (int y = 1; y < 3; y++) {
            chunk->generationSetBloc(x, baseHeight - y - worldAABBMin.y, z, SAND);
        }

    }
}

void FrozenOceanBiome::decorate(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight)
{
}

/// ====================== ///
/// ===== TaigaBiome ===== ///
/// ====================== ///

void TaigaBiome::applySurface(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight, glm::ivec3 worldAABBMin)
{
    chunk->generationSetBloc(x, baseHeight - 3 - worldAABBMin.y, z, DIRT);
    chunk->generationSetBloc(x, baseHeight - 2 - worldAABBMin.y, z, DIRT);
    chunk->generationSetBloc(x, baseHeight - 1 - worldAABBMin.y, z, DIRT);
    chunk->generationSetBloc(x, baseHeight - worldAABBMin.y, z, PODZOL);
}

void TaigaBiome::decorate(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight)
{
    if (chunk->getBloc(x, baseHeight + 2, z, true, false) != AIR) return;

    if(getRandom1000() < 5) addSpruceTree(chunk, x, z, baseHeight, getRandom1000() % 2);
}

void TaigaBiome::addSpruceTree(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight, int height)
{
    /*int floor = chunk->getBloc(x, baseHeight-1, z);
    int floorAbove = chunk->getBloc(x, baseHeight, z);
    if (floor != PODZOL) return;
    if (floorAbove != AIR) return;*/

    int actualHeight = 6 + 2 * height;
    chunk->generationSetBloc(x, baseHeight, z, DIRT);
    for (int dx = -3; dx <= 3; dx++) {
        for (int dz = -3; dz <= 3; dz++) {
            if (std::abs(dx) + std::abs(dz) <= 1) {
                chunk->generationSetBloc(x + dx, baseHeight + actualHeight - 1, z + dz, SPRUCE_LEAVES);
            }
            for (int i = 0; i <= height; i++) {
                if (std::abs(dx) + std::abs(dz) <= 2) {
                    chunk->generationSetBloc(x + dx, baseHeight + 4 + i * 2, z + dz, SPRUCE_LEAVES);
                }
                if (std::abs(dx) + std::abs(dz) <= 1) {
                    chunk->generationSetBloc(x + dx, baseHeight + 3 + i * 2, z + dz, SPRUCE_LEAVES);
                }
            }
            if (std::abs(dx) + std::abs(dz) <= 3) {
                chunk->generationSetBloc(x + dx, baseHeight + 2, z + dz, SPRUCE_LEAVES);
            }
        }
        for (int y = 1; y <= actualHeight-2; y++) chunk->generationSetBloc(x, baseHeight + y, z, SPRUCE_LOG);
        chunk->generationSetBloc(x, baseHeight + actualHeight, z, SPRUCE_LEAVES);
    }
}

/// ===================== ///
/// ===== MesaBiome ===== ///
/// ===================== ///

void MesaBiome::applySurface(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight, glm::ivec3 worldAABBMin)
{
    if (worldAABBMin.y + CHUNK_SIZE > MESA_START_TERRACOTTA) {
        for (int y = 15; y >= 0; y--) {
            if (worldAABBMin.y + y >= MESA_START_TERRACOTTA) {
                int b = chunk->getBloc(x, y, z);
                if ( b != AIR)
                chunk->generationSetBloc(x, y, z, getTerracottaPatternAt(worldAABBMin.y + y - MESA_START_TERRACOTTA));
            } else {
                break;
            }
        }
    }
    if (BlocDatabase::getInstance().isStoneOrStoneOre(chunk->getBloc(x, baseHeight, z))) {
        chunk->generationSetBloc(x, baseHeight - 4 - worldAABBMin.y, z, RED_SANDSTONE);
        chunk->generationSetBloc(x, baseHeight - 3 - worldAABBMin.y, z, RED_SANDSTONE);
        chunk->generationSetBloc(x, baseHeight - 2 - worldAABBMin.y, z, RED_SAND);
        chunk->generationSetBloc(x, baseHeight - 1 - worldAABBMin.y, z, RED_SAND);
        chunk->generationSetBloc(x, baseHeight - worldAABBMin.y, z, RED_SAND);
    }
}

void MesaBiome::decorate(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight)
{
}

int MesaBiome::getTerracottaPatternAt(int y)
{
    if (y < 0) return TERRACOTTA;
    if (y >= terracottaPattern.size()) return TERRACOTTA;
    return terracottaPattern[y];
}

/// ====================== ///
/// ===== DebugBiome ===== ///
/// ====================== ///

void DebugBiome::applySurface(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight, glm::ivec3 worldAABBMin)
{
    chunk->generationSetBloc(x, baseHeight - 1 - worldAABBMin.y, z, BEDROCK);
    chunk->generationSetBloc(x, baseHeight - worldAABBMin.y, z, BEDROCK);
}

void DebugBiome::decorate(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight)
{
}


/// ======================== ///
/// ===== BiomeManager ===== ///
/// ======================== ///

BiomeManager::BiomeManager(int groundLevel, int seed) : groundLevel(groundLevel), seed(seed)
{
    float freqScaling = 1.0f;
    temperature = FastNoiseLite();
    humidity = FastNoiseLite();
    erosion = FastNoiseLite();
    continentalness = FastNoiseLite();
    weirdness = FastNoiseLite();
    peaksAndValleys = FastNoiseLite();
    biomeNoise = FastNoiseLite();

    temperature.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    temperature.SetFrequency(0.003f * freqScaling);

    humidity.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    humidity.SetFrequency(0.005f * freqScaling);

    erosion.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    erosion.SetFrequency(0.0043f * freqScaling);
    erosion.SetFractalType(FastNoiseLite::FractalType_FBm);

    continentalness.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    continentalness.SetFrequency(0.004f * freqScaling);
    continentalness.SetFractalType(FastNoiseLite::FractalType_FBm);
    continentalness.SetFractalOctaves(4);
    continentalness.SetFractalLacunarity(2.1f);
    continentalness.SetFractalGain(0.55f);

    weirdness.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    weirdness.SetFrequency(0.004f * freqScaling);

    peaksAndValleys.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    peaksAndValleys.SetFrequency(0.0068f * freqScaling);
    peaksAndValleys.SetFractalType(FastNoiseLite::FractalType_Ridged);
    peaksAndValleys.SetFractalOctaves(2);
    peaksAndValleys.SetFractalLacunarity(3.50f);
    peaksAndValleys.SetFractalGain(0.0f);
    peaksAndValleys.SetFractalWeightedStrength(2.5f);

    biomeNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    biomeNoise.SetFrequency(0.01f);

    temperature.SetSeed(seed);
    humidity.SetSeed(seed-1);
    erosion.SetSeed(seed-2);
    continentalness.SetSeed(seed-3);
    weirdness.SetSeed(seed-4);
    peaksAndValleys.SetSeed(seed-4);
    biomeNoise.SetSeed(seed-5);

    continentalnessSpline = Spline(
        {-1.2f, -0.6f, -0.15f,  0.11f, 0.35f, 0.6f, 1.2f},
        {-50.0f,-40.0f,-3.0f,4.0f,10.0f,30.0f,40.0f}
    );

    erosionSpline = Spline(
        {-1.2f, -0.78f, -0.375f, -0.2225f, 0.05f, 0.4f, 0.6f, 1.0f, 1.2f},
        // {0.298f,0.349f,0.4f,0.447f,0.498f,0.549f,0.6f,0.647f,0.698f}
        {0.198f,0.249f,0.4f,0.447f,0.498f,0.749f,0.6f,0.847f,0.898f}
    );

    PVSpline = Spline(
        {-1.2f, -0.5f, -0.1f,  0.1f, 0.5f, 1.2f},
        {-1.0f,-0.9f,-0.1f,0.5f,0.9f,1.0f}
    );

    temperatureSpline = Spline(
        {-1.2f,-0.45f,-0.15f,0.2f,0.55f,1.2f},
        {-1.0f,-0.5f,-0.25f,0.25f,0.5f,1.0f}
    );
}

/**
 * @brief Add a biome to the manager and it's cellular noise. They should be set to CellularReturnType_Distance for nice blending.
 * 
 * @param biome 
 * @param noise 
 */
void BiomeManager::addBiome(std::unique_ptr<Biome> biome) {
    biomes.push_back(std::move(biome));
    std::sort(biomes.begin(), biomes.end(), [](const std::unique_ptr<Biome>& a, const std::unique_ptr<Biome>& b) {
        return a->getId() < b->getId();
    });
}

Biome* BiomeManager::getBiomeById(int id)
{
    return biomes[id].get();
}

Biome *BiomeManager::getBiome(int x, int z)
{
    //return getBiomeById(MESA_BIOME);
    float continentalness = getContinentalness(x, z);
    float weirdness = getWeirdness(x, z);
    float erosion = getErosion(x, z);
    int temperature = getTemperatureClass(getTemperature(x, z));
    if (continentalnessSpline.intervals[0].isInInterval(continentalness)) { // DEEP OCEAN
        if (temperature == TEMPERATURE_COLD) {
            return getBiomeById(FROZENOCEAN_BIOME);
        } else {
            return getBiomeById(OCEAN_BIOME);
        }
    } else if (continentalnessSpline.intervals[1].isInInterval(continentalness)) { // OCEAN
        if (temperature == TEMPERATURE_COLD) {
            return getBiomeById(FROZENOCEAN_BIOME);
        } else {
            return getBiomeById(BEACH_BIOME);
        }
    } else if (continentalnessSpline.intervals[2].isInInterval(continentalness)) { // COAST
        if (temperature == TEMPERATURE_COLD) {
            return getBiomeById(FROZENBEACH_BIOME);
        } else {
            return getBiomeById(BEACH_BIOME);
        }
    } else if (continentalnessSpline.intervals[3].isInInterval(continentalness)) { // NEAR INLAND
        switch(temperature) {
            case TEMPERATURE_COLD:
                if(weirdness > 0.25f) {
                    return getBiomeById(ICE_BIOME);
                } else {
                    return getBiomeById(TAIGA_BIOME);
                }
            case TEMPERATURE_TEMPERATE:
                return getBiomeById(PLAINS_BIOME);
            case TEMPERATURE_WARM:
                return getBiomeById(DESERT_BIOME);
        }
    } else if (continentalnessSpline.intervals[4].isInInterval(continentalness)) { // INLAND
        if (erosion > -0.05f) {
            switch(temperature) {
                case TEMPERATURE_COLD:
                    if(weirdness > -0.2f) {
                        return getBiomeById(ICE_BIOME);
                    } else {
                        return getBiomeById(TAIGA_BIOME);
                    }
                case TEMPERATURE_TEMPERATE:
                    if(weirdness > 0.1f) {
                        return getBiomeById(MUSHROOM_BIOME);
                    } else {
                        return getBiomeById(PLAINS_BIOME);
                    }
                case TEMPERATURE_WARM:
                    if(weirdness > 0.0f) {
                        return getBiomeById(DESERT_BIOME);
                    } else {
                        return getBiomeById(MESA_BIOME);
                    }
            }
        } else {
            switch(temperature) {
                case TEMPERATURE_COLD:
                    return getBiomeById(ICE_BIOME);
                case TEMPERATURE_TEMPERATE:
                    return getBiomeById(MOUNTAINS_BIOME);
                case TEMPERATURE_WARM:
                    return getBiomeById(CRISTALPEAKS_BIOME);
            }
        }
    } else if (continentalnessSpline.intervals[5].isInInterval(continentalness)) { // FAR INLAND
        if (erosion > 0.55f) {
            switch(temperature) {
                case TEMPERATURE_COLD:
                    return getBiomeById(ICE_BIOME);
                case TEMPERATURE_TEMPERATE:
                    if(weirdness > 0.3f) {
                        return getBiomeById(MUSHROOM_BIOME);
                    } else {
                        return getBiomeById(PLAINS_BIOME);
                    }
                case TEMPERATURE_WARM:
                    if(weirdness > 0.5f) {
                        return getBiomeById(DESERT_BIOME);
                    } else {
                        return getBiomeById(MESA_BIOME);
                    }
            }
        } else {
            switch(temperature) {
                case TEMPERATURE_COLD:
                    return getBiomeById(ICE_BIOME);
                case TEMPERATURE_TEMPERATE:
                    return getBiomeById(MOUNTAINS_BIOME);
                case TEMPERATURE_WARM:
                    return getBiomeById(CRISTALPEAKS_BIOME);
            }
        }
    }
    return getBiomeById(DEBUG_BIOME); // Default to the first biome if none match
}

float BiomeManager::getBaseHeight(int x, int z)
{
    //return continentalnessSpline.getValue(getContinentalness(x, z)) * (erosionSpline.getValue(getErosion(x,z))) + groundLevel;
    float cont = getContinentalness(x, z);
    float ret = continentalnessSpline.getValue(cont);
    ret += PVSpline.getValue(getPeaksAndValleys(x, z)) * PVSpline.getValue(getPeaksAndValleys(x, z)) * -8.0f ;
    if (cont > 0.0f) {
        ret *= (0.5f+erosionSpline.getValue(getErosion(x,z)));
    }
    ret += groundLevel;
    return ret;
}

int BiomeManager::getTemperatureClass(float temperature)
{
    if (temperature < -0.45f) return TEMPERATURE_COLD;
    if (temperature < 0.45f) return TEMPERATURE_TEMPERATE;
    return TEMPERATURE_WARM;
}

float BiomeManager::getTemperature(int x, int z) {
    return temperature.GetNoise((float) x, (float) z);
}
float BiomeManager::getHumidity(int x, int z)
{
    return humidity.GetNoise((float) x, (float) z);
}
float BiomeManager::getErosion(int x, int z) {
    return erosion.GetNoise((float) x, (float) z);
}
float BiomeManager::getContinentalness(int x, int z) {
    return continentalness.GetNoise((float) x, (float) z);
}
float BiomeManager::getWeirdness(int x, int z) {
    return weirdness.GetNoise((float) x, (float) z);
}
float BiomeManager::getPeaksAndValleys(int x, int z) {
    //return 1.0f - std::abs(3*std::abs(weirdness.GetNoise((float) x, (float) z))/2);
    return -1.0f*peaksAndValleys.GetNoise((float) x, (float) z)*peaksAndValleys.GetNoise((float) x, (float) z)+0.5f;
}