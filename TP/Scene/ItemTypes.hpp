#pragma once

#include <string>
#include <map>
#include "csv.h"
#include <iostream>

#include <Defines.hpp>

/**
 * @brief Class representing a block data
 * 
 */
class ItemData {
public:
    int id = 255;
    std::string name = "Error";
    float xTex = 15.0f * (TEXTUREATLAS_COORD_UNIT_X32 + TEXTUREATLAS_COORD_UNIT_OFFSET_X32 + TEXTUREATLAS_COORD_UNIT_OFFSET_X32) + TEXTUREATLAS_COORD_UNIT_OFFSET_X32; // UV.x coordinate for the side texture
    float yTex = 15.0f * (TEXTUREATLAS_COORD_UNIT_X32 + TEXTUREATLAS_COORD_UNIT_OFFSET_X32 + TEXTUREATLAS_COORD_UNIT_OFFSET_X32) + TEXTUREATLAS_COORD_UNIT_OFFSET_X32; // UV.y coordinate for the side texture

    ItemData() = default;

    ItemData(int id, std::string name,
        int xTex, int yTex)
        : id(id), name(name)
        {
            this->xTex = float(xTex) * (TEXTUREATLAS_COORD_UNIT_X32 + TEXTUREATLAS_COORD_UNIT_OFFSET_X32 + TEXTUREATLAS_COORD_UNIT_OFFSET_X32) + TEXTUREATLAS_COORD_UNIT_OFFSET_X32;
            this->yTex = float(yTex) * (TEXTUREATLAS_COORD_UNIT_X32 + TEXTUREATLAS_COORD_UNIT_OFFSET_X32 + TEXTUREATLAS_COORD_UNIT_OFFSET_X32) + TEXTUREATLAS_COORD_UNIT_OFFSET_X32;
        }
    
    ~ItemData() {}

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
class ItemDatabase {
private:
    std::map<int, ItemData> m_items;

    ItemDatabase() {
        std::string database = "../database/Blocs.csv";
        io::CSVReader<13> in(database);
        in.read_header(io::ignore_extra_column, "Id", "Name", "xTex", "yTex");
        int id;
        std::string name;
        float xTex, yTex;
        std::cout << "\nBloc database loading..." << std::endl;
        int count = 0;
        while (in.read_row(id, name, xTex, yTex)) {
            //std::cout << "BlocDatabase: id: " << id << ", name: " << name << ", Side UV: (" << xTexSide << ", " << yTexSide << "), Top UV: (" << xTexTop << ", " << yTexTop << "), Bottom UV: (" << xTexBottom << ", " << yTexBottom << "), Opaque: " << opaque << ", Solid: " << solid << ", Breakable: " << breakable << ", LightLevel: " << lightLevel << ", Ground: " << ground << std::endl;
            ItemData itemData(
                id, name,
                xTex, yTex
            );
            m_items[id] = itemData;
            count++;
        }
        std::cout << "Bloc database loaded, " << count << " blocs loaded." << std::endl;
        m_items[ERROR_BLOC] = ItemData();
    }

    // Prevent copying
    ItemDatabase(const ItemDatabase&) = delete;
    ItemDatabase& operator=(const ItemDatabase&) = delete;

    // Prevent moving
    ItemDatabase(ItemDatabase&&) = delete;
    ItemDatabase& operator=(ItemDatabase&&) = delete;
public:
    /**
     * @brief Get the Instance object
     * 
     * @return BlocDatabase& 
     */
    static ItemDatabase& getInstance() {
        static ItemDatabase instance;
        return instance;
    }

    /**
     * @brief Get the Bloc object
     * 
     * @param id The bloc id, you can use macros such as : AIR, STONE, DIRT, GRASS, PLANKS_OAK, ...
     * @return BlockData* 
     */
    ItemData* getItem(int id);


    /**
     * @brief Get the Tex Coords object
     * 
     * @param id The bloc id, you can use macros such as : AIR, STONE, DIRT, GRASS, PLANKS_OAK, ...
     * @param side defined macro you can use : FACE_SOUTH, FACE_NORTH, FACE_EAST, FACE_WEST, FACE_TOP, FACE_BOTTOM
     * @return std::pair<float, float> 
     */
    std::pair<float, float> getTexCoords(int id, unsigned char side);
};
