#include "WorldGenerator.hpp"


WorldGenerator::WorldGenerator() : rng(std::random_device{}()), treeChance(0, 100), ironRodChance(0, 1000) {
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

    biomeManager.addBiome(std::make_unique<PlainsBiome>(groundLevel, &baseHeightNoise), 0.75f);
    biomeManager.addBiome(std::make_unique<MoutainsBiome>(groundLevel, &mountainHeightNoise, &baseHeightNoise), 0.9f);
    biomeManager.addBiome(std::make_unique<DesertBiome>(groundLevel, &baseHeightNoise), 0.8f);
    biomeManager.addBiome(std::make_unique<WaterBiome>(groundLevel, &baseHeightNoise), 0.5f);
    biomeManager.addBiome(std::make_unique<IceBiome>(groundLevel, &baseHeightNoise), 0.65f);
}

void WorldGenerator::genereteProceduralChunk(World *world, VoxelChunk *chunk, int i, int j, int k) {
    //on appel la fonction generateTerrain pour générer le terrain à la coordonée i,j,k
    if (groundLevel + CHUNK_SIZE * 2 < j * CHUNK_SIZE) {
        // Skipping chunks too high to have any generated bloc
        return;
    }
    generateTerrain(world, chunk, i, j, k, groundLevel);
}

void WorldGenerator::setBaseStone(VoxelChunk *chunk, int x, int z, const glm::ivec3 &worldAABBMin, int baseHeight) {
    for (int y = 0; y < CHUNK_SIZE; y++) {
        if (y + worldAABBMin.y < baseHeight - 3) {
            chunk->generationSetBloc(x, y, z, STONE);
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
            std::vector<float> biomeWeights = biomeManager.getBiomeWeights(worldAABBMin.x + x, worldAABBMin.z + z);
            Biome* currentBiome = biomeManager.getDominantBiome(biomeWeights);
            int baseHeight = biomeManager.blendHeight(biomeWeights, x, z, worldAABBMin);

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

            // If surface level and biome is plains, add trees
            if (worldAABBMin.y <= baseHeight && worldAABBMin.y + chunk->m_sizeY > baseHeight && currentBiome->getId() == PLAINS_BIOME) {
                addTrees(chunk, x, z, baseHeight - worldAABBMin.y);
            }

            // If surface level, add iron rods
            if (worldAABBMin.y <= baseHeight && worldAABBMin.y + chunk->m_sizeY > baseHeight) {
                addIronRods(chunk, x, z, baseHeight - worldAABBMin.y);
            }

            // Bedrock at the bottom of the world
            if (worldAABBMin.y == 0) {
                chunk->generationSetBloc(x, 0, z, BEDROCK);
            }
        }
    }
}

void WorldGenerator::addTrees(VoxelChunk *chunk, int x, int z, int baseHeight) {

    if (treeChance(rng) < 1) { // 1% chance, no trees on mountains
        if (chunk->getBloc(x, baseHeight - 1, z) != GRASS) {
            return;
        }
        //std::cout << "Adding trees at (" << x << ", " << baseHeight << ", " << z << ")" << std::endl;
        //tree height
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

void WorldGenerator::addIronRods(VoxelChunk *chunk, int x, int z, int baseHeight) {

    if (ironRodChance(rng) < 1) {
        if (chunk->getBloc(x, baseHeight - 1, z) == AIR) {
            return;
        }
        int height = 3 + (treeChance(rng) % 4); // Random height between 3 and 5
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
