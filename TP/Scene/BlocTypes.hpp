#pragma once

#include <string>
#include <map>
#include "csv.h"
#include <iostream>

#include <Defines.hpp>

#define TEXTUREATLAS_UNIT 0.06250f

/**
 * @brief Class representing a block data
 * 
 */
class BlockData {
public:
    int id = 255;
    std::string name = "Error";
    float xTexSide = 15.0f * TEXTUREATLAS_UNIT; // UV.x coordinate for the side texture
    float yTexSide = 15.0f * TEXTUREATLAS_UNIT; // UV.y coordinate for the side texture
    float xTexTop = 15.0f * TEXTUREATLAS_UNIT; // UV.x coordinate for the top texture
    float yTexTop = 15.0f * TEXTUREATLAS_UNIT; // UV.y coordinate for the top texture
    float xTexBottom = 15.0f * TEXTUREATLAS_UNIT; // UV.x coordinate for the bottom texture
    float yTexBottom = 15.0f * TEXTUREATLAS_UNIT; // UV.y coordinate for the bottom texture
    float opaque = 1.0f; // Opacity of the block (1.0f = opaque, 0.0f = transparent)
    float solid = 1.0f; // True if the block is solid (can be walked on)
    bool breakable = false; // True if the block can be broken in survival
    int lightLevel = 0; // Light level of the block (0-15), 0 = no light, 15 = full light

    BlockData() = default;

    BlockData(int id, std::string name,
        int xTexSide, int yTexSide,
        int xTexTop, int yTexTop,
        int xTexBottom, int yTexBottom,
        int opaque, int solid, int breakable, int lightLevel)
        : id(id), name(name),
            xTexSide(xTexSide * TEXTUREATLAS_UNIT),
            yTexSide(yTexSide * TEXTUREATLAS_UNIT),
            xTexTop(xTexTop * TEXTUREATLAS_UNIT),
            yTexTop(yTexTop * TEXTUREATLAS_UNIT),
            xTexBottom(xTexBottom * TEXTUREATLAS_UNIT),
            yTexBottom(yTexBottom * TEXTUREATLAS_UNIT),
            opaque(opaque),
            solid(solid),
            breakable(breakable),
            lightLevel(lightLevel) {}
    
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
        io::CSVReader<12> in(database);
        in.read_header(io::ignore_extra_column, "Id", "Name", "xTexSide", "yTexSide", "xTexTop", "yTexTop", "xTexBottom", "yTexBottom", "Opaque", "Solid", "Breakable", "LightLevel");
        int id;
        std::string name;
        float xTexSide, yTexSide, xTexTop, yTexTop, xTexBottom, yTexBottom;
        int opaque, solid, breakable, lightLevel;
        std::cout << "\nBloc database loading..." << std::endl;
        while (in.read_row(id, name, xTexSide, yTexSide, xTexTop, yTexTop, xTexBottom, yTexBottom, opaque, solid, breakable, lightLevel)) {
            std::cout << "BlocDatabase: id: " << id << ", name: " << name << ", xTexSide: " << xTexSide << ", yTexSide: " << yTexSide << ", xTexTop: " << xTexTop << ", yTexTop: " << yTexTop << ", xTexBottom: " << xTexBottom << ", yTexBottom: " << yTexBottom << ", opaque: " << opaque << ", solid: " << solid << ", breakable: " << breakable << ", lightLevel: " << lightLevel << std::endl;
            BlockData blockData(
                id, name,
                xTexSide, yTexSide,
                xTexTop, yTexTop,
                xTexBottom, yTexBottom,
                opaque, solid, breakable, lightLevel
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

    int defaultLightLevel(int id) {
        return m_blocs[id].lightLevel;
    }

    bool isUnbreakable(int id) {
        return m_blocs[id].breakable==0;
    }

    bool isOpaque(int id) {
        return m_blocs[id].opaque==1;
    }

    bool isSolid(int id) {
        return m_blocs[id].solid==1.0f;
    }

    float solidValue(int id) {
        return m_blocs[id].solid;
    }

    bool isPartOfGround(int id) {
        return m_blocs[id].id==GRASS || m_blocs[id].id==DIRT || m_blocs[id].id==STONE || m_blocs[id].id==IRON_ORE || m_blocs[id].id==IRON_BLOCK || m_blocs[id].id==SAND || m_blocs[id].id==SANDSTONE || m_blocs[id].id==SNOW;
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
