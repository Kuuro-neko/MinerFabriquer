#include "WorldGenerator.hpp"

#include <utils/Math.hpp>   

WorldGenerator::WorldGenerator() : rng(std::random_device{}()) {
    // cave noise
    caveNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);
    caveNoise.SetFrequency(0.05f);
    caveNoise.SetSeed(seed);

    caveNoise2.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    caveNoise2.SetFrequency(0.06f);
    caveNoise2.SetSeed(seed);

    // Noise for ores
    oreNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    oreNoise.SetFrequency(0.7f);
    oreNoise.SetSeed(seed);

    bedrockNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    bedrockNoise.SetFrequency(0.8f);
    bedrockNoise.SetSeed(seed);

    rng.seed(seed);

    biomeManager.addBiome(std::make_unique<PlainsBiome>(groundLevel, seed));
    biomeManager.addBiome(std::make_unique<MoutainsBiome>(groundLevel, seed));
    biomeManager.addBiome(std::make_unique<DesertBiome>(groundLevel, seed));
    biomeManager.addBiome(std::make_unique<OceanBiome>(groundLevel, seed));
    biomeManager.addBiome(std::make_unique<IceBiome>(groundLevel, seed));
    biomeManager.addBiome(std::make_unique<CristalPeaksBiome>(groundLevel, seed));
    biomeManager.addBiome(std::make_unique<MushroomBiome>(groundLevel, seed));
    biomeManager.addBiome(std::make_unique<DebugBiome>(groundLevel, seed));
    biomeManager.addBiome(std::make_unique<BeachBiome>(groundLevel, seed));
    biomeManager.addBiome(std::make_unique<FrozenBeachBiome>(groundLevel, seed));
    biomeManager.addBiome(std::make_unique<FrozenOceanBiome>(groundLevel, seed));
    biomeManager.addBiome(std::make_unique<TaigaBiome>(groundLevel, seed));
    biomeManager.addBiome(std::make_unique<MesaBiome>(groundLevel, seed));
}

void WorldGenerator::setSeed(std::string seed)
{
    seedStr = seed;
    this->seed = stringToInt(seed);
    caveNoise2.SetSeed(this->seed);
    caveNoise.SetSeed(this->seed);
    oreNoise.SetSeed(this->seed);
    bedrockNoise.SetSeed(this->seed);
    biomeManager.setSeed(this->seed);
    std::cout << "Seed set to (str : " << seed << ") (int : " << this->seed << ")" << std::endl;
}

std::string WorldGenerator::getSeedStr()
{
    return seedStr;
}

void WorldGenerator::genereteProceduralChunk(std::shared_ptr<VoxelChunk> chunk, int i, int j, int k)
{
    //on appel la fonction generateTerrain pour générer le terrain à la coordonée i,j,k
    if (groundLevel + CHUNK_SIZE * 3 < j * CHUNK_SIZE) {
        // Skipping chunks too high to have any generated bloc
        return;
    }
    generateTerrain(chunk, i, j, k, groundLevel);
}

void WorldGenerator::decorateProceduralChunk(std::shared_ptr<VoxelChunk> chunk, int i, int j, int k) {
    //on appel la fonction generateTerrain pour générer le terrain à la coordonée i,j,k
    if (groundLevel + CHUNK_SIZE * 3 < j * CHUNK_SIZE) {
        // Skipping chunks too high to have any generated bloc
        return;
    }
    decorateTerrain(chunk, i, j, k, groundLevel);
}

void WorldGenerator::setBaseStone(std::shared_ptr<VoxelChunk> chunk, int x, int z, const glm::ivec3 &worldAABBMin, int baseHeight) {
    for (int y = 0; y < CHUNK_SIZE; y++) {
        if (y + worldAABBMin.y <= baseHeight) {
            chunk->generationSetBloc(x, y, z, STONE, false, false);
        } else if (y + worldAABBMin.y < WATER_LEVEL) {
            int distanceToWaterLevel = WATER_LEVEL - (y + worldAABBMin.y);
            chunk->generationSetBloc(x, y, z, WATER, false, false);
            chunk->setLightLevel(x, y, z, std::clamp(15 - distanceToWaterLevel, 0, 15));
        }
    }
}

/**
 * @brief Generate the terrain of a chunk at the given chunks coordinates.
 * 
 * @param chunk 
 * @param i 
 * @param j 
 * @param k 
 * @param groundLevel 
 */
void WorldGenerator::generateTerrain(std::shared_ptr<VoxelChunk> chunk, int i, int j, int k, int groundLevel) {
    for (int x = 0; x < CHUNK_SIZE; ++x) {
        for (int z = 0; z < CHUNK_SIZE; ++z) {
            // worldAABBMin.xyz are the world pos of the min pos of the chunk
            // so x and z are local to the chunk and x + worldAABBMin.x and z + worldAABBMin.z are the world pos (useful for the noises)
            glm::ivec3 worldAABBMin = glm::ivec3(
                i * CHUNK_SIZE,
                j * CHUNK_SIZE,
                k * CHUNK_SIZE
            );

            Biome* currentBiome = biomeManager.getBiome(worldAABBMin.x + x, worldAABBMin.z + z);
            int baseHeight = biomeManager.getBaseHeight(worldAABBMin.x + x, worldAABBMin.z + z);

            // Set the base stone shape
            setBaseStone(chunk, x, z, worldAABBMin, baseHeight);
            

            // Apply the biome surface
            currentBiome->applySurface(chunk, x, z, baseHeight, worldAABBMin);

            // Generate ores in stone blocs
            for (int y = 0; y < CHUNK_SIZE; y++) {
                if (chunk->getBloc(x, y, z) == STONE) { // Replaces only stone
                    float wX = x + i * CHUNK_SIZE;
                    float wY = y + j * CHUNK_SIZE;
                    float wZ = z + k * CHUNK_SIZE;
                    float oreNoiseValue = oreNoise.GetNoise(wX, wY, wZ);
                    if (oreNoiseValue > IRON_THRESHOLD) {
                        chunk->generationSetBloc(x, y, z, IRON_ORE);
                    }
                    oreNoiseValue = oreNoise.GetNoise(wX + 100, wY + 100, wZ + 100);
                    if (oreNoiseValue > COAL_THRESHOLD) {
                        chunk->generationSetBloc(x, y, z, COAL_ORE);
                    }
                    oreNoiseValue = oreNoise.GetNoise(wX + 200, wY + 200, wZ + 200);
                    if (oreNoiseValue > GOLD_THRESHOLD) {
                        chunk->generationSetBloc(x, y, z, GOLD_ORE);
                    }
                    oreNoiseValue = oreNoise.GetNoise(wX + 300, wY + 300, wZ + 300);
                    if (oreNoiseValue > DIAMOND_THRESHOLD) {
                        chunk->generationSetBloc(x, y, z, DIAMOND_ORE);
                    }
                    oreNoiseValue = oreNoise.GetNoise(wX + 400, wY + 400, wZ + 400);
                    if (oreNoiseValue > EMERALD_THRESHOLD) {
                        chunk->generationSetBloc(x, y, z, EMERALD_ORE);
                    }
                }
            }

            // Generate caves on ground blocs
            for (int y = 0; y < CHUNK_SIZE; y++) { // Replaces ground blocs
                if (BlocDatabase::getInstance().isPartOfGround(chunk->getBloc(x, y, z))) {
                    float caveNoiseValue = caveNoise.GetNoise((float) x + i * CHUNK_SIZE,
                                                               (float) y + j * CHUNK_SIZE,
                                                               (float) z + k * CHUNK_SIZE);
                    float caveNoiseValue2 = caveNoise2.GetNoise((float) x + i * CHUNK_SIZE,
                                                                (float) y + j * CHUNK_SIZE,
                                                                (float) z + k * CHUNK_SIZE);
                    float value = caveNoiseValue + caveNoiseValue2;
                    float check = CAVE_BASE_THRESHOLD - (worldAABBMin.y + y ) * CAVE_DEPTH_SCALING_FACTOR;
                    if (worldAABBMin.y + y > groundLevel) {
                        check -= (worldAABBMin.y + y ) * 0.05f;
                    }
                    if (value < check) {
                        chunk->generationSetBloc(x, y, z, AIR);
                    }
                }
            }
        }
    }
}

void WorldGenerator::decorateTerrain(std::shared_ptr<VoxelChunk> chunk, int i, int j, int k, int groundLevel) {
    for (int x = 0; x < CHUNK_SIZE; ++x) {
        for (int z = 0; z < CHUNK_SIZE; ++z) {
            // worldAABBMin.xyz are the world pos of the min pos of the chunk
            // so x and z are local to the chunk and x + worldAABBMin.x and z + worldAABBMin.z are the world pos (useful for the noises)
            glm::ivec3 worldAABBMin = glm::ivec3(
                i * CHUNK_SIZE,
                j * CHUNK_SIZE,
                k * CHUNK_SIZE
            );

            // Compute the baseHeight : "heightmap" of the current biome based on its blend with other biomes
            // std::vector<float> biomeWeights = biomeManager.getBiomeWeights(worldAABBMin.x + x, worldAABBMin.z + z);
            // Biome* currentBiome = biomeManager.getDominantBiome(biomeWeights);
            // int baseHeight = biomeManager.blendHeight(biomeWeights, x, z, worldAABBMin);

            Biome* currentBiome = biomeManager.getBiome(worldAABBMin.x + x, worldAABBMin.z + z);
            int baseHeight = biomeManager.getBaseHeight(worldAABBMin.x + x, worldAABBMin.z + z);

            currentBiome->decorate(chunk, x, z, baseHeight);

            // Bedrock at the bottom of the world
            if (worldAABBMin.y == 0) {
                chunk->generationSetBloc(x, 0, z, BEDROCK);
                float noise = bedrockNoise.GetNoise((float) x + i * CHUNK_SIZE, (float) z + k * CHUNK_SIZE);
                if (noise > -0.6f) {
                    chunk->generationSetBloc(x, 1, z, BEDROCK);
                }
                if (noise < -0.15f) {
                    chunk->generationSetBloc(x, 2, z, BEDROCK);
                }
            }
        }
    }
}