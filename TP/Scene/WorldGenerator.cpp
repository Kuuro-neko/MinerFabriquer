#include "WorldGenerator.hpp"


WorldGenerator::WorldGenerator() : rng(std::random_device{}()), treeChance(0, 100) {
    // Perlin noise for base terrain
    baseNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    baseNoise.SetFrequency(0.1f);
    // Perlin noise for mountains
    mountainNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    mountainNoise.SetFrequency(0.05f);


}

void WorldGenerator::genereteProceduralChunk(VoxelChunk *world, int i, int j) {
    //on appel la fonction generateTerrain pour générer le terrain à la coordonée i,0,j
    generateTerrain(world, i, j, groundLevel);
}

void WorldGenerator::generateTerrain(VoxelChunk *chunk, int i, int j, int groundLevel) {
    //on génère le terrain en fonction du bruit de base et du bruit de montagne
    for (int x = 0; x < chunk->m_sizeX; ++x) {
        for (int z = 0; z < chunk->m_sizeZ; ++z) {
            int baseHeight = groundLevel + static_cast<int>(
                    baseNoise.GetNoise((float) x + i * CHUNK_SIZE, (float) z + j * CHUNK_SIZE) * 5);
            //lastLayer - 4 -> BEDROCK
            chunk->generationSetBloc(x, groundLevel - 4, z, BEDROCK);

            //groundLevel - 3  -> STONE
            for (int y = groundLevel - 3; y < baseHeight - 2; y++) {
                chunk->generationSetBloc(x, y, z, STONE);
            }

            //groundLevel - 2 -> DIRT
            //groundLevel - 1 -> GRASS
            chunk->generationSetBloc(x, baseHeight - 2, z, DIRT);
            chunk->generationSetBloc(x, baseHeight - 1, z, GRASS);

            int mountainHeight = static_cast<int>(
                    mountainNoise.GetNoise((float) x + i * CHUNK_SIZE, (float) z + j * CHUNK_SIZE) * 10);
            if (mountainHeight > 0) {
                for (int y = baseHeight; y < baseHeight + mountainHeight; y++) {
                    chunk->generationSetBloc(x, y, z, STONE);
                    if (chunk->getBloc(x, y-1, z) == GRASS) {
                        chunk->generationSetBloc(x, y - 1, z, DIRT); // Add grass on top of mountains
                    }
                }
            }

            addTrees(chunk, x, z, baseHeight);
        }
    }


    chunk->generateMesh();
}

void WorldGenerator::addTrees(VoxelChunk *chunk, int x, int z, int baseHeight) {

    if (treeChance(rng) < 1) { // 1% chance, no trees on mountains
        std::cout << "Adding trees at (" << x << ", " << baseHeight << ", " << z << ")" << std::endl;
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