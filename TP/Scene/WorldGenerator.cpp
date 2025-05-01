#include "WorldGenerator.hpp"


WorldGenerator::WorldGenerator() : rng(std::random_device{}()), treeChance(0, 100) {
    // Perlin noise for base terrain
    baseNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    baseNoise.SetFrequency(0.1f);
    // Perlin noise for mountains
    mountainNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    mountainNoise.SetFrequency(0.03f);
    // Noise for caves
    caveNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);
    caveNoise.SetFrequency(0.05f);

    caveNoise2.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    caveNoise2.SetFrequency(0.06f);

    // Noise for ores
    oreNoise.SetNoiseType(FastNoiseLite::NoiseType_ValueCubic);
    oreNoise.SetFrequency(0.7f);

    // Noise for biomes
    biomeNoise.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
    biomeNoise.SetFrequency(0.75f);
    biomeNoise.SetCellularReturnType(FastNoiseLite::CellularReturnType_CellValue);

}

int WorldGenerator::getBiome(int x, int z) {
    // Get the biome based on the noise value
    float noiseValue = biomeNoise.GetNoise((float) x*0.01, (float) z*0.011);
    if (noiseValue < -0.6f) {
        return DESERT;
    } else  {
        return PLAINS;
    }
}

void WorldGenerator::genereteProceduralChunk(VoxelChunk *world, int i, int j, int k) {
    //on appel la fonction generateTerrain pour générer le terrain à la coordonée i,j,k
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

            int baseHeight = groundLevel + static_cast<int>(
                    baseNoise.GetNoise((float) x + worldX, (float) z + worldZ) * 5);

            int mountainHeight = static_cast<int>(
                mountainNoise.GetNoise((float) x + i * CHUNK_SIZE, (float) z + k * CHUNK_SIZE) * 10);
                
            //groundLevel - 3  -> STONE
            for (int y = worldY; y < baseHeight - 2; y++) {
                chunk->generationSetBloc(x, y - worldY, z, STONE);
            }
            //groundLevel - 2 -> DIRT
            //groundLevel - 1 -> GRASS
            if (getBiome(worldX + x, worldZ + z) == DESERT) {
                chunk->generationSetBloc(x, baseHeight - 6 - worldY, z, SANDSTONE);
                chunk->generationSetBloc(x, baseHeight - 5 - worldY, z, SANDSTONE);
                chunk->generationSetBloc(x, baseHeight - 4 - worldY, z, SANDSTONE);
                chunk->generationSetBloc(x, baseHeight - 3 - worldY, z, SAND);
                chunk->generationSetBloc(x, baseHeight - 2 - worldY, z, SAND);
                chunk->generationSetBloc(x, baseHeight - 1 - worldY, z, SAND);
            } else {
                chunk->generationSetBloc(x, baseHeight - 3 - worldY, z, DIRT);
                chunk->generationSetBloc(x, baseHeight - 2 - worldY, z, DIRT);
                chunk->generationSetBloc(x, baseHeight - 1 - worldY, z, GRASS);


                if (mountainHeight > 0) {
                    for (int y = baseHeight; y < baseHeight + mountainHeight; y++) {
                        chunk->generationSetBloc(x, y - worldY, z, STONE);
                        if (chunk->getBloc(x, y-1 - worldY, z) == GRASS) {
                            chunk->generationSetBloc(x, y - 1 - worldY, z, DIRT); // Add grass on top of mountains
                        }
                    }
                }
            }


            // Ores
            for (int y = 0; y < chunk->m_sizeY; y++) {
                if (chunk->getBloc(x, y, z) == STONE) { // Replaces stone
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
                    if (value < -0.6f + (worldY + y ) * 0.01f ) {
                        chunk->generationSetBloc(x, y, z, AIR);
                    }
                }
            }

            // If surface level and not top of moutain
            if (worldY <= baseHeight && worldY + chunk->m_sizeY > baseHeight && mountainHeight <=0) {
                addTrees(chunk, x, z, baseHeight - worldY);
            }

            // If surface level
            if (worldY <= baseHeight && worldY + chunk->m_sizeY > baseHeight) {
                if(mountainHeight <=0) addIronRods(chunk, x, z, baseHeight - worldY);
                else addIronRods(chunk, x, z, baseHeight + mountainHeight - worldY);
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

    if (treeChance(rng) < 0.5) { // 1% chance, no trees on mountains
        if (chunk->getBloc(x, baseHeight - 1, z) == AIR) {
            return;
        }
        int height = 3 + (treeChance(rng) % 3); // Random height between 3 and 5
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
