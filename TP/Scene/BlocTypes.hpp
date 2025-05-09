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
    bool ground = false; // True if the block is a ground block (dirt, grass, etc.)

    BlockData() = default;

    BlockData(int id, std::string name,
        int xTexSide, int yTexSide,
        int xTexTop, int yTexTop,
        int xTexBottom, int yTexBottom,
        int opaque, int solid, int breakable, int lightLevel, int ground)
        : id(id), name(name),
            opaque(opaque),
            solid(solid),
            breakable(breakable),
            lightLevel(lightLevel),
            ground(ground) {
                this->xTexSide = float(xTexSide) * (TEXTUREATLAS_COORD_UNIT_X32 + TEXTUREATLAS_COORD_UNIT_OFFSET_X32 + TEXTUREATLAS_COORD_UNIT_OFFSET_X32) + TEXTUREATLAS_COORD_UNIT_OFFSET_X32;
                this->yTexSide = float(yTexSide) * (TEXTUREATLAS_COORD_UNIT_X32 + TEXTUREATLAS_COORD_UNIT_OFFSET_X32 + TEXTUREATLAS_COORD_UNIT_OFFSET_X32) + TEXTUREATLAS_COORD_UNIT_OFFSET_X32;
                this->xTexTop = float(xTexTop) * (TEXTUREATLAS_COORD_UNIT_X32 + TEXTUREATLAS_COORD_UNIT_OFFSET_X32 + TEXTUREATLAS_COORD_UNIT_OFFSET_X32) + TEXTUREATLAS_COORD_UNIT_OFFSET_X32;
                this->yTexTop = float(yTexTop) * (TEXTUREATLAS_COORD_UNIT_X32 + TEXTUREATLAS_COORD_UNIT_OFFSET_X32 + TEXTUREATLAS_COORD_UNIT_OFFSET_X32) + TEXTUREATLAS_COORD_UNIT_OFFSET_X32;
                this->xTexBottom = float(xTexBottom) * (TEXTUREATLAS_COORD_UNIT_X32 + TEXTUREATLAS_COORD_UNIT_OFFSET_X32 + TEXTUREATLAS_COORD_UNIT_OFFSET_X32) + TEXTUREATLAS_COORD_UNIT_OFFSET_X32;
                this->yTexBottom = float(yTexBottom) * (TEXTUREATLAS_COORD_UNIT_X32 + TEXTUREATLAS_COORD_UNIT_OFFSET_X32 + TEXTUREATLAS_COORD_UNIT_OFFSET_X32) + TEXTUREATLAS_COORD_UNIT_OFFSET_X32;
            }
    
    ~BlockData() {}

    /**
     * @brief Get the Tex Coords object from a side
     * 
     * @param side defined macro you can use : FACE_SOUTH, FACE_NORTH, FACE_EAST, FACE_WEST, FACE_TOP, FACE_BOTTOM
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
        io::CSVReader<13> in(database);
        in.read_header(io::ignore_extra_column, "Id", "Name", "xTexSide", "yTexSide", "xTexTop", "yTexTop", "xTexBottom", "yTexBottom", "Opaque", "Solid", "Breakable", "LightLevel", "Ground");
        int id;
        std::string name;
        float xTexSide, yTexSide, xTexTop, yTexTop, xTexBottom, yTexBottom;
        int opaque, solid, breakable, lightLevel, ground;
        std::cout << "\nBloc database loading..." << std::endl;
        int count = 0;
        while (in.read_row(id, name, xTexSide, yTexSide, xTexTop, yTexTop, xTexBottom, yTexBottom, opaque, solid, breakable, lightLevel, ground)) {
            //std::cout << "BlocDatabase: id: " << id << ", name: " << name << ", Side UV: (" << xTexSide << ", " << yTexSide << "), Top UV: (" << xTexTop << ", " << yTexTop << "), Bottom UV: (" << xTexBottom << ", " << yTexBottom << "), Opaque: " << opaque << ", Solid: " << solid << ", Breakable: " << breakable << ", LightLevel: " << lightLevel << ", Ground: " << ground << std::endl;
            BlockData blockData(
                id, name,
                xTexSide, yTexSide,
                xTexTop, yTexTop,
                xTexBottom, yTexBottom,
                opaque, solid, breakable, lightLevel, ground
            );
            m_blocs[id] = blockData;
            count++;
        }
        std::cout << "Bloc database loaded, " << count << " blocs loaded." << std::endl;
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
        return id != -2 && m_blocs[id].solid;
    }

    bool isPartOfGround(int id) {
        return m_blocs[id].ground;
    }

    bool isStoneOrStoneOre(int id) {
        return id == STONE || id == IRON_ORE || id == COAL_ORE || id == GOLD_ORE || id == DIAMOND_ORE || id == EMERALD_ORE;
    }

    /**
     * @brief Get the Tex Coords object
     * 
     * @param id The bloc id, you can use macros such as : AIR, STONE, DIRT, GRASS, PLANKS_OAK, ...
     * @param side defined macro you can use : FACE_SOUTH, FACE_NORTH, FACE_EAST, FACE_WEST, FACE_TOP, FACE_BOTTOM
     * @return std::pair<float, float> 
     */
    std::pair<float, float> getTexCoords(int id, unsigned char side);
};
