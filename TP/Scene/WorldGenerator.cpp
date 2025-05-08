#include "WorldGenerator.hpp"


WorldGenerator::WorldGenerator() : rng(std::random_device{}()), bedrockRng(0, 100) {
    // Perlin noise for base terrain
    baseHeightNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    baseHeightNoise.SetFrequency(0.1f);
    baseHeightNoise.SetSeed(seed);
    // Perlin noise for mountains
    mountainHeightNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    mountainHeightNoise.SetFrequency(0.03f);
    mountainHeightNoise.SetSeed(seed);
    // Noise for caves
    caveNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);
    caveNoise.SetFrequency(0.05f);
    caveNoise.SetSeed(seed);

    caveNoise2.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    caveNoise2.SetFrequency(0.06f);
    caveNoise2.SetSeed(seed);

    // Noise for ores
    oreNoise.SetNoiseType(FastNoiseLite::NoiseType_ValueCubic);
    oreNoise.SetFrequency(0.7f);
    oreNoise.SetSeed(seed);
    
    // Noise for water holes
    waterHolesHeightNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    waterHolesHeightNoise.SetFrequency(0.02f);
    waterHolesHeightNoise.SetSeed(seed);

    rng.seed(seed);

    biomeManager.addBiome(std::make_unique<PlainsBiome>(groundLevel, &baseHeightNoise, seed), 0.008f);
    biomeManager.addBiome(std::make_unique<MoutainsBiome>(groundLevel, &mountainHeightNoise, &baseHeightNoise, seed), 0.01f);
    biomeManager.addBiome(std::make_unique<DesertBiome>(groundLevel, &baseHeightNoise, seed), 0.009f);
    biomeManager.addBiome(std::make_unique<OceanBiome>(groundLevel, &baseHeightNoise, seed), 0.013f);
    biomeManager.addBiome(std::make_unique<IceBiome>(groundLevel, &baseHeightNoise, seed), 0.009f);
    biomeManager.addBiome(std::make_unique<CristalPeaksBiome>(groundLevel, &baseHeightNoise, seed), 0.012f);
    biomeManager.addBiome(std::make_unique<MushroomBiome>(groundLevel, &baseHeightNoise, seed), 0.009f);
    biomeManager.addBiome(std::make_unique<DebugBiome>(groundLevel, &baseHeightNoise, seed), 0.01f);
}

void WorldGenerator::genereteProceduralChunk(World *world, VoxelChunk *chunk, int i, int j, int k) {
    //on appel la fonction generateTerrain pour générer le terrain à la coordonée i,j,k
    if (groundLevel + CHUNK_SIZE * 2 < j * CHUNK_SIZE) {
        // Skipping chunks too high to have any generated bloc
        return;
    }
    generateTerrain(world, chunk, i, j, k, groundLevel);
}

void WorldGenerator::decorateProceduralChunk(World *world, VoxelChunk *chunk, int i, int j, int k) {
    //on appel la fonction generateTerrain pour générer le terrain à la coordonée i,j,k
    if (groundLevel + CHUNK_SIZE * 2 < j * CHUNK_SIZE) {
        // Skipping chunks too high to have any generated bloc
        return;
    }
    decorateTerrain(world, chunk, i, j, k, groundLevel);
}

void WorldGenerator::setBaseStone(VoxelChunk *chunk, int x, int z, const glm::ivec3 &worldAABBMin, int baseHeight) {
    for (int y = 0; y < CHUNK_SIZE; y++) {
        if (y + worldAABBMin.y < baseHeight) {
            chunk->generationSetBloc(x, y, z, STONE);
        } else if (y + worldAABBMin.y < groundLevel-1) {
            chunk->generationSetBloc(x, y, z, WATER);
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
void WorldGenerator::generateTerrain(World *world, VoxelChunk *chunk, int i, int j, int k, int groundLevel) {
    for (int x = 0; x < chunk->m_sizeX; ++x) {
        for (int z = 0; z < chunk->m_sizeZ; ++z) {
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

            // Set the base stone shape
            setBaseStone(chunk, x, z, worldAABBMin, baseHeight);
            

            // Apply the biome surface
            currentBiome->applySurface(chunk, x, z, baseHeight, worldAABBMin);

            // Generate ores in stone blocs
            for (int y = 0; y < chunk->m_sizeY; y++) {
                if (chunk->getBloc(x, y, z) == STONE) { // Replaces only stone
                    float oreNoiseValue = oreNoise.GetNoise((float) x + i * CHUNK_SIZE,
                                                            (float) y + j * CHUNK_SIZE,
                                                            (float) z + k * CHUNK_SIZE);
                    if (oreNoiseValue > IRON_THRESHOLD) {
                        chunk->generationSetBloc(x, y, z, IRON_ORE);
                    }
                }
            }

            // Generate caves on ground blocs
            for (int y = 0; y < chunk->m_sizeY; y++) { // Replaces ground blocs
                if (BlocDatabase::getInstance().isPartOfGround(chunk->getBloc(x, y, z))) {
                    float caveNoiseValue = caveNoise.GetNoise((float) x + i * CHUNK_SIZE,
                                                               (float) y + j * CHUNK_SIZE,
                                                               (float) z + k * CHUNK_SIZE);
                    float caveNoiseValue2 = caveNoise2.GetNoise((float) x + i * CHUNK_SIZE,
                                                                (float) y + j * CHUNK_SIZE,
                                                                (float) z + k * CHUNK_SIZE);
                    float value = caveNoiseValue + caveNoiseValue2;
                    float check = CAVE_BASE_THRESHOLD + (worldAABBMin.y + y ) * CAVE_DEPTH_SCALING_FACTOR;
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

void WorldGenerator::decorateTerrain(World *world, VoxelChunk *chunk, int i, int j, int k, int groundLevel) {
    for (int x = 0; x < chunk->m_sizeX; ++x) {
        for (int z = 0; z < chunk->m_sizeZ; ++z) {
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
                int r = bedrockRng(rng);
                if(r < 50) {
                    chunk->generationSetBloc(x, 1, z, BEDROCK);
                }
                if(r < 25) {
                    chunk->generationSetBloc(x, 2, z, BEDROCK);
                }
            }
        }
    }
}