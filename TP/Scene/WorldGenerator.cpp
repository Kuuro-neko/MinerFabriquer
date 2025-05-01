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

    // Noise for biomes
    plainsNoise.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
    plainsNoise.SetFrequency(0.75f);
    plainsNoise.SetCellularReturnType(FastNoiseLite::CellularReturnType_CellValue);
    plainsNoise.SetCellularDistanceFunction(FastNoiseLite::CellularDistanceFunction_Manhattan);
    plainsNoise.SetSeed(seed);

    moutainNoise.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
    moutainNoise.SetFrequency(0.9f);
    moutainNoise.SetCellularReturnType(FastNoiseLite::CellularReturnType_CellValue);
    moutainNoise.SetCellularDistanceFunction(FastNoiseLite::CellularDistanceFunction_Manhattan);
    moutainNoise.SetSeed(seed);

    // Perlin noise for desert
    desertNoise.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
    desertNoise.SetFrequency(0.8f);
    desertNoise.SetCellularReturnType(FastNoiseLite::CellularReturnType_CellValue);
    desertNoise.SetCellularDistanceFunction(FastNoiseLite::CellularDistanceFunction_Manhattan);
    desertNoise.SetSeed(seed);

    rng.seed(seed);
}

std::vector<float> WorldGenerator::getBiomeWeights(int x, int z) {
    float nx = x * 0.01f;
    float nz = z * 0.011f;

    float n1 = plainsNoise.GetNoise(nx*1.2f, nz);               // plains
    float n2 = moutainNoise.GetNoise(nx + 150, nz*0.9f + 150);  // desert
    float n3 = desertNoise.GetNoise(nx*1.1f + 250, nz*0.8f - 130);  // mountain

    // Center values around 0.5
    float plainsRaw = 1.0f - std::abs(n1);
    float desertRaw = 1.0f - std::abs(n2);
    float mountainRaw = 1.0f - std::abs(n3);

    // Make them positive and curved — exp or square
    plainsRaw = std::pow(plainsRaw, 2.0f);
    desertRaw = std::pow(desertRaw, 2.0f);
    mountainRaw = std::pow(mountainRaw, 2.0f);

    std::vector<float> weights = {plainsRaw, desertRaw, mountainRaw};
    float sum = 0.0f;
    for (float w : weights) sum += w;
    for (float &w : weights) w /= sum;

    return weights;
}


int WorldGenerator::getBiome(std::vector<float> weights) {
    float maxWeight = weights[0];
    int biome = 0;
    for (int i = 1; i < weights.size(); ++i) {
        if (weights[i] > maxWeight) {
            maxWeight = weights[i];
            biome = i;
        }
    }
    return biome;
}

void WorldGenerator::genereteProceduralChunk(VoxelChunk *world, int i, int j, int k) {
    //on appel la fonction generateTerrain pour générer le terrain à la coordonée i,j,k
    if (groundLevel + CHUNK_SIZE * 2 < j * CHUNK_SIZE) {
        // Skipping chunks too high to have any generated bloc
        return;
    }
    generateTerrain(world, i, j, k, groundLevel);
}

void WorldGenerator::generateTerrain(VoxelChunk *chunk, int i, int j, int k, int groundLevel) {
    //on génère le terrain en fonction du bruit de base et du bruit de montagne
    for (int x = 0; x < chunk->m_sizeX; ++x) {
        for (int z = 0; z < chunk->m_sizeZ; ++z) {
            // worldX Y Z are the world pos of the min pos of the chunk
            // so x and z are local to the chunk and x + worldX and z + worldZ are the world pos (useful for the noises)
            int worldX = i * CHUNK_SIZE;
            int worldY = j * CHUNK_SIZE;
            int worldZ = k * CHUNK_SIZE;

            std::vector<float> biomeWeights = getBiomeWeights(worldX + x, worldZ + z);
            int biome = getBiome(biomeWeights);

            float baseNoiseValue = baseHeightNoise.GetNoise((float) x + i * CHUNK_SIZE, (float) z + k * CHUNK_SIZE);

            float mountainNoiseValue = mountainHeightNoise.GetNoise((float) 260 + x + i * CHUNK_SIZE, (float) 500 + z + k * CHUNK_SIZE);

            int baseHeight = 0;

            // Calculate base height based on biome
            float plainsHeight = groundLevel + std::max(baseNoiseValue * 5.0f, 0.f);
            float desertHeight = groundLevel + std::max(baseNoiseValue, 0.f);  // Flatter for desert
            float mountainHeight = groundLevel + std::max(mountainNoiseValue * mountainNoiseValue * 80.0f + 3.f, 0.f); // Higher mountains

            // Blend the heights
            baseHeight = plainsHeight * biomeWeights[0] +
                        desertHeight * biomeWeights[1] +
                        mountainHeight * biomeWeights[2];

            // Stone everywhere
            for (int y = 0; y < CHUNK_SIZE; y++) {
                if (y + worldY < baseHeight - 3) {
                    chunk->generationSetBloc(x, y, z, STONE);
                }
            }

            switch(biome) {
                case PLAINS:
                    chunk->generationSetBloc(x, baseHeight - 3 - worldY, z, DIRT);
                    chunk->generationSetBloc(x, baseHeight - 2 - worldY, z, DIRT);
                    chunk->generationSetBloc(x, baseHeight - 1 - worldY, z, GRASS);
                    break;
                case DESERT:
                    chunk->generationSetBloc(x, baseHeight - 6 - worldY, z, SANDSTONE);
                    chunk->generationSetBloc(x, baseHeight - 5 - worldY, z, SANDSTONE);
                    chunk->generationSetBloc(x, baseHeight - 4 - worldY, z, SANDSTONE);
                    chunk->generationSetBloc(x, baseHeight - 3 - worldY, z, SAND);
                    chunk->generationSetBloc(x, baseHeight - 2 - worldY, z, SAND);
                    chunk->generationSetBloc(x, baseHeight - 1 - worldY, z, SAND);
                    break;
                case MOUNTAINS:
                    int snowheight = 15 + baseNoiseValue * 8;
                    if (baseHeight > groundLevel + snowheight) {
                        for (int y = 0; y < 3; y++) {
                            chunk->generationSetBloc(x, baseHeight - y - 2 - worldY, z, SNOW);
                        }
                    } else {
                        chunk->generationSetBloc(x, baseHeight - 3 - worldY, z, STONE);
                        chunk->generationSetBloc(x, baseHeight - 2 - worldY, z, STONE);
                        chunk->generationSetBloc(x, baseHeight - 1 - worldY, z, STONE);
                    }
                break;
            }


            // Ores
            for (int y = 0; y < chunk->m_sizeY; y++) {
                if (chunk->getBloc(x, y, z) == STONE) { // Replaces only stone
                    float oreNoiseValue = oreNoise.GetNoise((float) x + i * CHUNK_SIZE,
                                                            (float) y + j * CHUNK_SIZE,
                                                            (float) z + k * CHUNK_SIZE);
                    if (oreNoiseValue > 0.28f) {
                        chunk->generationSetBloc(x, y, z, IRON_ORE);
                    }
                }
            }

            // Caves
            for (int y = 0; y < chunk->m_sizeY; y++) { // Replaces ground blocs
                if (BlocDatabase::getInstance().isPartOfGround(chunk->getBloc(x, y, z))) {
                    float caveNoiseValue = caveNoise.GetNoise((float) x + i * CHUNK_SIZE,
                                                               (float) y + j * CHUNK_SIZE,
                                                               (float) z + k * CHUNK_SIZE);
                    float caveNoiseValue2 = caveNoise2.GetNoise((float) x + i * CHUNK_SIZE,
                                                                (float) y + j * CHUNK_SIZE,
                                                                (float) z + k * CHUNK_SIZE);
                    float value = caveNoiseValue + caveNoiseValue2;
                    float check = -0.6f + (worldY + y ) * 0.01f;
                    if (worldY + y > groundLevel) {
                        check -= (worldY + y ) * 0.05f;
                    }
                    if (value < check) {
                        chunk->generationSetBloc(x, y, z, AIR);
                    }
                }
            }

            // If surface level and not top of moutain
            if (worldY <= baseHeight && worldY + chunk->m_sizeY > baseHeight && getBiome(biomeWeights) == PLAINS) {
                addTrees(chunk, x, z, baseHeight - worldY);
            }

            // If surface level
            if (worldY <= baseHeight && worldY + chunk->m_sizeY > baseHeight) {
                addIronRods(chunk, x, z, baseHeight - worldY);
            }

            //lastLayer - 4 -> BEDROCK
            if (worldY == 0) {
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
