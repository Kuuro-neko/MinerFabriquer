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
#define BEDROCK 7
#define GLOWSTONE 8
#define WATER 9
#define SAND 10
#define IRON_ORE 11
#define IRON_BLOCK 12
#define SANDSTONE 13

#define ERROR_BLOC 255

// Block types, bit mask for each side
#define BLOC_FRONT 1
#define BLOC_BACK 2
#define BLOC_LEFT 4
#define BLOC_RIGHT 8
#define BLOC_TOP 16
#define BLOC_BOTTOM 32

#define TEXTUREATLAS_UNIT 0.06250f

/**
 * @brief Class representing a block data
 * 
 */
class BlockData {
public:
    int id = 255;
    std::string name = "Error";
    float xTexSide = 15.0f * TEXTUREATLAS_UNIT;
    float yTexSide = 15.0f * TEXTUREATLAS_UNIT;
    float xTexTop = 15.0f * TEXTUREATLAS_UNIT;
    float yTexTop = 15.0f * TEXTUREATLAS_UNIT;
    float xTexBottom = 15.0f * TEXTUREATLAS_UNIT;
    float yTexBottom = 15.0f * TEXTUREATLAS_UNIT;
    float opaque = 1.0f;
    bool breakable = false;

    BlockData() = default;

    BlockData(int id, std::string name,
        int xTexSide, int yTexSide,
        int xTexTop, int yTexTop,
        int xTexBottom, int yTexBottom,
        int opaque, int breakable)
        : id(id), name(name),
            xTexSide(xTexSide * TEXTUREATLAS_UNIT),
            yTexSide(yTexSide * TEXTUREATLAS_UNIT),
            xTexTop(xTexTop * TEXTUREATLAS_UNIT),
            yTexTop(yTexTop * TEXTUREATLAS_UNIT),
            xTexBottom(xTexBottom * TEXTUREATLAS_UNIT),
            yTexBottom(yTexBottom * TEXTUREATLAS_UNIT),
            opaque(opaque),
            breakable(breakable) {}
    
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
        io::CSVReader<10> in(database);
        in.read_header(io::ignore_extra_column, "Id", "Name", "xTexSide", "yTexSide", "xTexTop", "yTexTop", "xTexBottom", "yTexBottom", "Opaque", "Breakable");
        int id;
        std::string name;
        float xTexSide, yTexSide, xTexTop, yTexTop, xTexBottom, yTexBottom;
        int opaque, breakable;
        while (in.read_row(id, name, xTexSide, yTexSide, xTexTop, yTexTop, xTexBottom, yTexBottom, opaque, breakable)) {
            std::cout << "BlocDatabase: id: " << id << ", name: " << name << ", xTexSide: " << xTexSide << ", yTexSide: " << yTexSide << ", xTexTop: " << xTexTop << ", yTexTop: " << yTexTop << ", xTexBottom: " << xTexBottom << ", yTexBottom: " << yTexBottom << ", opaque: " << opaque << ", breakable: " << breakable << std::endl;
            BlockData blockData(
                id, name,
                xTexSide, yTexSide,
                xTexTop, yTexTop,
                xTexBottom, yTexBottom,
                opaque, breakable
            );
            m_blocs[id] = blockData;
        }
        m_blocs[ERROR_BLOC] = BlockData();
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

    //
    bool isAir(int id) {
        return m_blocs[id].id==0;
    }

    bool isUnbreakable(int id) {
        return m_blocs[id].breakable==0;
    }

    bool isOpaque(int id) {
        return m_blocs[id].opaque==1;
    }

    bool isPartOfGround(int id) {
        return m_blocs[id].id==GRASS || m_blocs[id].id==DIRT || m_blocs[id].id==STONE || m_blocs[id].id==IRON_ORE || m_blocs[id].id==IRON_BLOCK || m_blocs[id].id==SAND;
    }

    /**
     * @brief Get the Tex Coords object
     * 
     * @param id The bloc id, you can use macros such as : AIR, STONE, DIRT, GRASS, PLANKS_OAK, ...
     * @param side defined macro you can use : BLOC_FRONT, BLOC_BACK, BLOC_LEFT, BLOC_RIGHT, BLOC_TOP, BLOC_BOTTOM
     * @return std::pair<float, float> 
     */
    std::pair<float, float> getTexCoords(int id, unsigned char side);
};
