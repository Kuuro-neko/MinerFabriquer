#pragma once

#include <TP/Scene/VoxelChunk.hpp>
#include <random>
#include <Defines.hpp>
#include <utils/Math.hpp>

#define TEMPERATURE_COLD 0
#define TEMPERATURE_WARM 1
#define TEMPERATURE_TEMPERATE 2

class Biome {
    private:
        int groundLevel = 40;
        int id;
        int seed;
        std::mt19937 rng;
        std::uniform_int_distribution<int> random1000;
        std::uniform_real_distribution<float> randomFloat;
    public:
        Biome(int groundLevel, int id, int seed) : groundLevel(groundLevel), id(id), seed(seed) {
            rng.seed(seed+id);
            random1000 = std::uniform_int_distribution<int>(0, 1000);
            randomFloat = std::uniform_real_distribution<float>(0.0f, 1.0f);
        }

        /**
         * @brief Apply the surface of the biome to the chunk at the given coordinates.
         * 
         * @param chunk The chunk being generated
         * @param x Relative x coordinate in the chunk
         * @param z Relative z coordinate in the chunk
         * @param baseHeight Computed using the BiomerManager.blendHeight function before
         * @param worldAABBMin The chunks' world coordinates of it's min corner
         */
        virtual void applySurface(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight, glm::ivec3 worldAABBMin) = 0;
        virtual void decorate(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight) = 0;
        virtual ~Biome() = default;

        /**
         * @brief Get the id of the biome.
         * 
         * @return int 
         */
        inline int getId() const { return id; }
        inline int getGroundLevel() const { return groundLevel; }
        inline void setSeed(int seed) { this->seed = seed; }
        inline int getSeed() const { return seed; }
        inline int getRandom1000() { return random1000(rng); }
        inline float getRandomFloat() { return randomFloat(rng); }
        inline std::mt19937& getRNG() { return rng; }
};

/// ================== ///
/// ===== Biomes ===== ///
/// ================== ///

class PlainsBiome : public Biome {
    public:
        PlainsBiome(int groundLevel, int seed) : Biome(groundLevel, PLAINS_BIOME, seed) {}
        void applySurface(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight, glm::ivec3 worldAABBMin) override;
        void decorate(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight) override;
};

class MoutainsBiome : public Biome {
    private:
        FastNoiseLite snowNoise;
    public:
        MoutainsBiome(int groundLevel, int seed) : Biome(groundLevel, MOUNTAINS_BIOME, seed) {
            snowNoise = FastNoiseLite();
            snowNoise.SetSeed(getSeed());
            snowNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
            snowNoise.SetFrequency(0.1f);
        }
        void applySurface(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight, glm::ivec3 worldAABBMin) override;
        void decorate(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight) override;
        int getSnowHeight(float baseNoise);
};

class DesertBiome : public Biome {
    public:
        DesertBiome(int groundLevel, int seed) : Biome(groundLevel, DESERT_BIOME, seed) {}
        void applySurface(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight, glm::ivec3 worldAABBMin) override;
        void decorate(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight) override; 
};

class OceanBiome : public Biome {
    private:
    FastNoiseLite coralNoise;
    public :
        OceanBiome(int groundLevel, int seed) : Biome(groundLevel, OCEAN_BIOME, seed) {
            coralNoise = FastNoiseLite();
            coralNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
            coralNoise.SetFrequency(0.1f);
            coralNoise.SetSeed(getSeed());
        }
        void applySurface(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight, glm::ivec3 worldAABBMin) override;
        void decorate(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight) override;
};

class IceBiome : public Biome {
    public:
        IceBiome(int groundLevel, int seed) : Biome(groundLevel, ICE_BIOME, seed) {}
        void applySurface(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight, glm::ivec3 worldAABBMin) override;
        void decorate(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight) override;
        void addIceSpike(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight, int height, int radius);
};

class CristalPeaksBiome : public Biome {
    private:
        FastNoiseLite amethystNoise;
    public:
        CristalPeaksBiome(int groundLevel, int seed) : Biome(groundLevel, CRISTALPEAKS_BIOME, seed) {
            amethystNoise.SetNoiseType(FastNoiseLite::NoiseType_ValueCubic);
            amethystNoise.SetFrequency(0.05f);
            amethystNoise.SetSeed(seed);
            amethystNoise.SetFractalType(FastNoiseLite::FractalType_FBm);
            amethystNoise.SetFractalOctaves(1);
            amethystNoise.SetFractalLacunarity(5.0f);
            amethystNoise.SetFractalGain(5.0f);
            amethystNoise.SetFractalWeightedStrength(0.5f);
        }
        void applySurface(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight, glm::ivec3 worldAABBMin) override;
        void decorate(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight) override;
};

class MushroomBiome : public Biome {
    private:
        FastNoiseLite coralVineNoise;
    public:
        MushroomBiome(int groundLevel, int seed) : Biome(groundLevel, MUSHROOM_BIOME, seed) {
            coralVineNoise = FastNoiseLite();
            coralVineNoise.SetNoiseType(FastNoiseLite::NoiseType_ValueCubic);
            coralVineNoise.SetFrequency(0.25f);
            coralVineNoise.SetSeed(getSeed());
            coralVineNoise.SetFractalType(FastNoiseLite::FractalType_PingPong);
        }
        void applySurface(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight, glm::ivec3 worldAABBMin) override;
        void decorate(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight) override;
        void addMushroomCup(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight, int stemHeight, int stemRadius, float capRadius, int stemMaterial, int capMaterial);
        void addMushroomReverseU(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight, int stemHeight, float capRadius, int stemMaterial, int capMaterial);
};

class BeachBiome : public Biome {
    public:
        BeachBiome(int groundLevel, int seed) : Biome(groundLevel, BEACH_BIOME, seed) {}
        void applySurface(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight, glm::ivec3 worldAABBMin) override;
        void decorate(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight) override; 
};

class FrozenBeachBiome : public Biome {
    public:
        FrozenBeachBiome(int groundLevel, int seed) : Biome(groundLevel, FROZENBEACH_BIOME, seed) {
        }
        void applySurface(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight, glm::ivec3 worldAABBMin) override;
        void decorate(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight) override; 
};

class FrozenOceanBiome : public Biome {
    private:
        FastNoiseLite iceNoise;
    public:
        FrozenOceanBiome(int groundLevel, int seed) : Biome(groundLevel, FROZENOCEAN_BIOME, seed) {
            iceNoise = FastNoiseLite();
            iceNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
            iceNoise.SetFrequency(0.05f);
            iceNoise.SetSeed(getSeed());
            iceNoise.SetFractalType(FastNoiseLite::FractalType_FBm);
            iceNoise.SetFractalOctaves(2);
            iceNoise.SetFractalLacunarity(2.0f);
            iceNoise.SetFractalGain(5.0f);
            iceNoise.SetFractalWeightedStrength(5.0f);
        }
        void applySurface(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight, glm::ivec3 worldAABBMin) override;
        void decorate(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight) override; 
};

class TaigaBiome : public Biome {
    public:
        TaigaBiome(int groundLevel, int seed) : Biome(groundLevel, TAIGA_BIOME, seed) {}
        void applySurface(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight, glm::ivec3 worldAABBMin) override;
        void decorate(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight) override;
        void addSpruceTree(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight, int height);
};

class MesaBiome : public Biome {
    private:
        std::vector<int> terracottaPattern;
    public:
        MesaBiome(int groundLevel, int seed) : Biome(groundLevel, MESA_BIOME, seed) {
            terracottaPattern = {
                TERRACOTTA, ORANGE_TERRACOTTA, ORANGE_TERRACOTTA, BLACK_TERRACOTTA,
                BLACK_TERRACOTTA, WHITE_TERRACOTTA, TERRACOTTA, TERRACOTTA,
                YELLOW_TERRACOTTA, GRAY_TERRACOTTA, WHITE_TERRACOTTA, WHITE_TERRACOTTA,
                BLACK_TERRACOTTA, BLACK_TERRACOTTA, ORANGE_TERRACOTTA, ORANGE_TERRACOTTA,
                TERRACOTTA, TERRACOTTA, WHITE_TERRACOTTA, WHITE_TERRACOTTA,
                YELLOW_TERRACOTTA, GRAY_TERRACOTTA, RED_TERRACOTTA, TERRACOTTA,
                TERRACOTTA, TERRACOTTA, BROWN_TERRACOTTA, BROWN_TERRACOTTA,
                ORANGE_TERRACOTTA, ORANGE_TERRACOTTA, BLACK_TERRACOTTA, BLACK_TERRACOTTA,
                WHITE_TERRACOTTA, WHITE_TERRACOTTA, GRAY_TERRACOTTA, GRAY_TERRACOTTA,
                TERRACOTTA, TERRACOTTA, YELLOW_TERRACOTTA, YELLOW_TERRACOTTA,
                BROWN_TERRACOTTA, BROWN_TERRACOTTA, ORANGE_TERRACOTTA, ORANGE_TERRACOTTA,
            };
        }
        void applySurface(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight, glm::ivec3 worldAABBMin) override;
        void decorate(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight) override;
        int getTerracottaPatternAt(int y);
};

class DebugBiome : public Biome {
    public:
        DebugBiome(int groundLevel, int seed) : Biome(groundLevel, DEBUG_BIOME, seed) {
        }
        void applySurface(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight, glm::ivec3 worldAABBMin) override;
        void decorate(std::shared_ptr<VoxelChunk> chunk, int x, int z, int baseHeight) override; 
};

/// ======================== ///
/// ===== BiomeManager ===== ///
/// ======================== ///

class BiomeManager {
    std::vector<std::unique_ptr<Biome>> biomes;

    int groundLevel;
    int seed;

    FastNoiseLite temperature;
    FastNoiseLite humidity;
    FastNoiseLite erosion;
    FastNoiseLite continentalness;
    FastNoiseLite weirdness;
    FastNoiseLite peaksAndValleys;
    FastNoiseLite biomeNoise;

    Spline continentalnessSpline;
    Spline erosionSpline;
    Spline PVSpline;
    Spline temperatureSpline;
public:
    BiomeManager(int groundLevel, int seed);
    void addBiome(std::unique_ptr<Biome> biome);

    Biome* getBiomeById(int id);
    Biome* getBiome(int x, int z);
    float getBaseHeight(int x, int z);

    int getTemperatureClass(float temperature);

    float getTemperature(int x, int z);
    float getHumidity(int x, int z);
    float getErosion(int x, int z);
    float getContinentalness(int x, int z);
    float getWeirdness(int x, int z);
    float getPeaksAndValleys(int x, int z);

    void setSeed(int seed);

    BiomeManager(const BiomeManager&) = delete;
    BiomeManager& operator=(const BiomeManager&) = delete;
};