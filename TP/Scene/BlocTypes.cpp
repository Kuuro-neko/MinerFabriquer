#include <TP/Scene/BlocTypes.hpp>

std::string getTexturePath(int bloc, int side) {
    switch (bloc) {
        case STONE:
            //return "../textures/blocs/stone.png";
            return "../textures/rock.ppm";
        case DIRT:
            //return "../textures/blocs/dirt.png";
            return "../textures/grass.ppm";
        case GRASS:
            if (side == BLOC_SIDE) {
                return "../textures/blocs/grass_side.png";
            } else {
                return "../textures/blocs/grass_top.png";
            }
        case PLANKS_OAK:
            return "../textures/blocs/planks_oak.png";
        case LOG_OAK:
            if (side == BLOC_SIDE) {
                return "../textures/blocs/log_oak_side.png";
            } else {
                return "../textures/blocs/log_oak_top.png";
            }
        case LEAVES_OAK:
            return "../textures/blocs/leaves_oak.tga";
        default:
            return "";
    }
}