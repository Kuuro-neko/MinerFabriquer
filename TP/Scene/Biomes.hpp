#pragma once

#include <TP/Scene/VoxelChunk.hpp>

#include <Defines.hpp>
class Biome {
    private:
        int groundLevel = 40;
        FastNoiseLite* noise;
        int id;
    public:
        Biome(int groundLevel, FastNoiseLite* noise, int id) : groundLevel(groundLevel), noise(noise), id(id) {}
        
        /**
         * @brief Calculate the height of the biome at the given coordinates.
         * 
         * @param x World x coordinate
         * @param z World z coordinate
         * @return float 
         */
        virtual float calculateHeight(float x, float z) = 0;

        /**
         * @brief Apply the surface of the biome to the chunk at the given coordinates.
         * 
         * @param chunk The chunk being generated
         * @param x Relative x coordinate in the chunk
         * @param z Relative z coordinate in the chunk
         * @param baseHeight Computed using the BiomerManager.blendHeight function before
         * @param worldAABBMin The chunks' world coordinates of it's min corner
         */
        virtual void applySurface(VoxelChunk* chunk, int x, int z, int baseHeight, glm::ivec3 worldAABBMin) = 0;
        virtual void decorate(VoxelChunk* chunk, int x, int z, int baseHeight) = 0;
        virtual ~Biome() = default;

        /**
         * @brief Get the id of the biome.
         * 
         * @return int 
         */
        inline int getId() const { return id; }
        inline int getGroundLevel() const { return groundLevel; }
        inline FastNoiseLite* getNoise() const { return noise; }
};

/// ================== ///
/// ===== Biomes ===== ///
/// ================== ///

class PlainsBiome : public Biome {
    public:
        PlainsBiome(int groundLevel, FastNoiseLite* noise) : Biome(groundLevel, noise, PLAINS_BIOME) {}
        float calculateHeight(float x, float z) override;
        void applySurface(VoxelChunk* chunk, int x, int z, int baseHeight, glm::ivec3 worldAABBMin) override;
        void decorate(VoxelChunk* chunk, int x, int z, int baseHeight) override;
};

class MoutainsBiome : public Biome {
    private:
        FastNoiseLite* snowNoise;
    public:
        MoutainsBiome(int groundLevel, FastNoiseLite* noise, FastNoiseLite* snowNoise) : Biome(groundLevel, noise, MOUNTAINS_BIOME), snowNoise(snowNoise) {}
        float calculateHeight(float x, float z) override;
        void applySurface(VoxelChunk* chunk, int x, int z, int baseHeight, glm::ivec3 worldAABBMin) override;
        void decorate(VoxelChunk* chunk, int x, int z, int baseHeight) override;
        int getSnowHeight(float baseNoise);
};

class DesertBiome : public Biome {
    public:
        DesertBiome(int groundLevel, FastNoiseLite* noise) : Biome(groundLevel, noise, DESERT_BIOME) {}
        float calculateHeight(float x, float z) override;
        void applySurface(VoxelChunk* chunk, int x, int z, int baseHeight, glm::ivec3 worldAABBMin) override;
        void decorate(VoxelChunk* chunk, int x, int z, int baseHeight) override;
        
};

class WaterBiome : public Biome {
    private:
        FastNoiseLite* waterNoise;
        int getMinNeighborHeight(VoxelChunk *chunk, int x, int z, glm::ivec3 worldAABBMin);

    public :
        WaterBiome(int groundLevel, FastNoiseLite *noise) : Biome(groundLevel, noise, WATER_BIOME) {}
        float calculateHeight(float x, float z) override;
        void applySurface(VoxelChunk* chunk, int x, int z, int baseHeight, glm::ivec3 worldAABBMin) override;
        void decorate(VoxelChunk* chunk, int x, int z, int baseHeight) override;
};

/// ======================== ///
/// ===== BiomeManager ===== ///
/// ======================== ///

class BiomeManager {
    std::vector<std::unique_ptr<Biome>> biomes;
    std::vector<FastNoiseLite> noises;
    int groundLevel;
    int seed;
public:
    BiomeManager(int groundLevel, int seed);
    void addBiome(std::unique_ptr<Biome> biome, float cellularNoiseFrequency);
    std::vector<float> getBiomeWeights(int x, int z);
    Biome* getDominantBiome(const std::vector<float>& weights);
    float blendHeight(const std::vector<float> &weights, int x, int z, glm::ivec3 worldAABBMin);
};