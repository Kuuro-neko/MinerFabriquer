#pragma once

#include <string>

#define AIR 0
#define STONE 1
#define DIRT 2
#define GRASS 3
#define PLANKS_OAK 4
#define LOG_OAK 5
#define LEAVES_OAK 6

#define BLOC_SIDE 0
#define BLOC_TOP 1

std::string getTexturePath(int bloc, int side);