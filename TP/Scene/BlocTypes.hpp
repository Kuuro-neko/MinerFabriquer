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

class BlockData {
public:
    int id = 0;
    std::string name = "Air";
    float xTexSide = 0.0f;
    float yTexSide = 0.0f;
    float xTexTop = 0.0f;
    float yTexTop = 0.0f;

    BlockData() = default;

    BlockData(int id, std::string name, int xTexSide, int yTexSide, int xTexTop, int yTexTop)
        : id(id), name(name),
            xTexSide(xTexSide * TEXTUREATLAS_UNIT),
            yTexSide(yTexSide * TEXTUREATLAS_UNIT),
            xTexTop(xTexTop * TEXTUREATLAS_UNIT),
            yTexTop(yTexTop * TEXTUREATLAS_UNIT) {}
    
    ~BlockData() {}

    std::pair<float, float> getTexCoords(unsigned char side) {
        if (side == BLOC_TOP || side == BLOC_BOTTOM) {
            return std::make_pair(xTexTop, yTexTop);
        } else {
            return std::make_pair(xTexSide, yTexSide);
        }
    }
};

class BlocDatabase {
private:
    std::map<int, BlockData> m_blocs;

    BlocDatabase() {
        std::string database = "../database/Blocs.csv";
        io::CSVReader<6> in(database);
        in.read_header(io::ignore_extra_column, "Id", "Name", "xTexSide", "yTexSide", "xTexTop", "yTexTop");
        int id;
        std::string name;
        float xTexSide, yTexSide, xTexTop, yTexTop;
        while (in.read_row(id, name, xTexSide, yTexSide, xTexTop, yTexTop)) {
            std::cout << "BlocDatabase: id: " << id << ", name: " << name << ", xTexSide: " << xTexSide << ", yTexSide: " << yTexSide << ", xTexTop: " << xTexTop << ", yTexTop: " << yTexTop << std::endl;
            BlockData blockData(id, name, xTexSide, yTexSide, xTexTop, yTexTop);
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
    static BlocDatabase& getInstance() {
        static BlocDatabase instance;
        return instance;
    }

    BlockData* getBloc(int id) {
        auto it = m_blocs.find(id);
        if (it != m_blocs.end()) {
            return &it->second;
        }
        return nullptr;
    }

    std::pair<float, float> getTexCoords(int id, unsigned char side) {
        auto it = m_blocs.find(id);
        if (it != m_blocs.end()) {
            return it->second.getTexCoords(side);
        }
        return std::make_pair(0.0f, 0.0f);
    }
};
