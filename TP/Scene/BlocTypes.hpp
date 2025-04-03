#pragma once

#include <string>
#include <map>
#include "csv.h"
#include <iostream>

#define AIR 0
#define STONE 1
#define DIRT 2
#define GRASS 3
#define PLANKS_OAK 4
#define LOG_OAK 5
#define LEAVES_OAK 6

// Block types, bit mask for each side
#define BLOC_FRONT 1
#define BLOC_BACK 2
#define BLOC_LEFT 4
#define BLOC_RIGHT 8
#define BLOC_TOP 16
#define BLOC_BOTTOM 32

#define TEXTUREATLAS_UNIT 0.0625f

/**
 * @brief Class representing a block data
 * 
 */
class BlockData {
public:
    int id = 0;
    std::string name = "Air";
    float xTexSide = 0.0f;
    float yTexSide = 0.0f;
    float xTexTop = 0.0f;
    float yTexTop = 0.0f;
    float xTexBottom = 0.0f;
    float yTexBottom = 0.0f;
    float opaque = 1.0f;

    BlockData() = default;

    BlockData(int id, std::string name,
        int xTexSide, int yTexSide,
        int xTexTop, int yTexTop,
        int xTexBottom, int yTexBottom,
        int opaque)
        : id(id), name(name),
            xTexSide(xTexSide * TEXTUREATLAS_UNIT),
            yTexSide(yTexSide * TEXTUREATLAS_UNIT),
            xTexTop(xTexTop * TEXTUREATLAS_UNIT),
            yTexTop(yTexTop * TEXTUREATLAS_UNIT),
            xTexBottom(xTexBottom * TEXTUREATLAS_UNIT),
            yTexBottom(yTexBottom * TEXTUREATLAS_UNIT),
            opaque(opaque) {}
    
    ~BlockData() {}

    /**
     * @brief Get the Tex Coords object from a side
     * 
     * @param side defined macro you can use : BLOC_FRONT, BLOC_BACK, BLOC_LEFT, BLOC_RIGHT, BLOC_TOP, BLOC_BOTTOM
     * @return std::pair<float, float> 
     */
    std::pair<float, float> getTexCoords(unsigned char side);
};

/**
 * @brief Singleton class to manage the block database
 * 
 */
class BlocDatabase {
private:
    std::map<int, BlockData> m_blocs;

    BlocDatabase() {
        std::string database = "../database/Blocs.csv";
        io::CSVReader<9> in(database);
        in.read_header(io::ignore_extra_column, "Id", "Name", "xTexSide", "yTexSide", "xTexTop", "yTexTop", "xTexBottom", "yTexBottom", "Opaque");
        int id;
        std::string name;
        float xTexSide, yTexSide, xTexTop, yTexTop, xTexBottom, yTexBottom;
        int opaque;
        while (in.read_row(id, name, xTexSide, yTexSide, xTexTop, yTexTop, xTexBottom, yTexBottom, opaque)) {
            std::cout << "BlocDatabase: id: " << id << ", name: " << name << ", xTexSide: " << xTexSide << ", yTexSide: " << yTexSide << ", xTexTop: " << xTexTop << ", yTexTop: " << yTexTop << ", xTexBottom: " << xTexBottom << ", yTexBottom: " << yTexBottom << ", opaque: " << opaque << std::endl;
            BlockData blockData(
                id, name,
                xTexSide, yTexSide,
                xTexTop, yTexTop,
                xTexBottom, yTexBottom,
                opaque
            );
            m_blocs[id] = blockData;
        }
    }

    // Prevent copying
    BlocDatabase(const BlocDatabase&) = delete;
    BlocDatabase& operator=(const BlocDatabase&) = delete;

    // Prevent moving
    BlocDatabase(BlocDatabase&&) = delete;
    BlocDatabase& operator=(BlocDatabase&&) = delete;
public:
    /**
     * @brief Get the Instance object
     * 
     * @return BlocDatabase& 
     */
    static BlocDatabase& getInstance() {
        static BlocDatabase instance;
        return instance;
    }

    /**
     * @brief Get the Bloc object
     * 
     * @param id The bloc id, you can use macros such as : AIR, STONE, DIRT, GRASS, PLANKS_OAK, ...
     * @return BlockData* 
     */
    BlockData* getBloc(int id);

    /**
     * @brief Get the Tex Coords object
     * 
     * @param id The bloc id, you can use macros such as : AIR, STONE, DIRT, GRASS, PLANKS_OAK, ...
     * @param side defined macro you can use : BLOC_FRONT, BLOC_BACK, BLOC_LEFT, BLOC_RIGHT, BLOC_TOP, BLOC_BOTTOM
     * @return std::pair<float, float> 
     */
    std::pair<float, float> getTexCoords(int id, unsigned char side);
};
