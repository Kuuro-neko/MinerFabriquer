#pragma once
#include <string>
// ===== Maths ===== //

#define EPSILON 0.001f // To slightly move the texture coordinates to avoid neighboring textures bleeding into each other, doesn't work well

// ===== Fast access ===== //

#define RENDERER_DISTANCE 12
#define GENERATION_DISTANCE 8

// ===== Generation & chunks ===== //

#define GROUND_LEVEL 65.0f
#define WATER_LEVEL 63
#define MESA_START_TERRACOTTA 67

#define CHUNK_SIZE 16
#define BLOC_SIZE 1

#define OUT_OF_BOUNDS_BLOC -2

#define GENERATION_RADIUS_X 8 // X radius of initial generation
#define GENERATION_SIZE_Y 8 // Y height of initial generation
#define GENERATION_RADIUS_Z 8 // Z radius of initial generation
#define GENERATION_SIZE_X  8
#define GENERATION_SIZE_Z  8


// ===== Bloc database ===== //

#define TEXTUREATLAS_COORD_UNIT 0.0625f

#define TEXTUREATLAS_COORD_UNIT_OFFSET_X32 0.00625f
#define TEXTUREATLAS_COORD_UNIT_X32 0.05f

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
#define SNOW 14
#define ICE 15
#define AMETHYST 16
#define SMOOTH_BASALT 17
#define CALCITE 18
#define ANDESITE 19
#define BRAIN_CORAL 20
#define BUBBLE_CORAL 21
#define END_STONE 22
#define MOSS 23
#define MUSHROOM_STEM 24
#define BROWN_MUSHROOM 25
#define MYCELIUM 26
#define RED_MUSHROOM 27
#define SPRUCE_PLANKS 28
#define SPRUCE_LOG 29
#define SPRUCE_LEAVES 30
#define JUNGLE_PLANKS 31
#define JUNGLE_LOG 32
#define JUNGLE_LEAVES 33
#define RED_SAND 34
#define RED_SANDSTONE 35
#define TERRACOTTA 36
#define BLACK_TERRACOTTA 37
#define BLUE_TERRACOTTA 38
#define BROWN_TERRACOTTA 39
#define CYAN_TERRACOTTA 40
#define GRAY_TERRACOTTA 41
#define GREEN_TERRACOTTA 42
#define LIGHTBLUE_TERRACOTTA 43
#define LIGHTGRAY_TERRACOTTA 44
#define LIME_TERRACOTTA 45
#define MAGENTA_TERRACOTTA 46
#define ORANGE_TERRACOTTA 47
#define PINK_TERRACOTTA 48
#define PURPLE_TERRACOTTA 49
#define RED_TERRACOTTA 50
#define WHITE_TERRACOTTA 51
#define YELLOW_TERRACOTTA 52
#define MANGROVE_PLANKS 53
#define MANGROVE_LOG 54
#define MANGROVE_LEAVES 55
#define MUD 56
#define CLAY 57
#define GOLD_ORE 58
#define GOLD_BLOCK 59
#define DIAMOND_ORE 60
#define DIAMOND_BLOCK 61
#define COPPER_ORE 62
#define COPPER_BLOCK 63
#define COAL_ORE 64
#define EMERALD_ORE 65
#define EMERALD_BLOCK 66
#define MAGMA 67
#define OBSIDIAN 68
#define BLACKSTONE 69
#define GILDED_BLACKSTONE 70
#define PODZOL 71

#define ERROR_BLOC 255

// Block face, bit mask for each side
#define FACE_SOUTH 1
#define FACE_NORTH 2
#define FACE_EAST 4
#define FACE_WEST 8
#define FACE_TOP 16
#define FACE_BOTTOM 32

// ===== Biomes ===== //

#define PLAINS_BIOME 0
#define MOUNTAINS_BIOME 1
#define DESERT_BIOME 2
#define OCEAN_BIOME 3
#define ICE_BIOME 4
#define CRISTALPEAKS_BIOME 5
#define MUSHROOM_BIOME 6
#define DEBUG_BIOME 7
#define BEACH_BIOME 8
#define FROZENBEACH_BIOME 9
#define FROZENOCEAN_BIOME 10
#define TAIGA_BIOME 11
#define MESA_BIOME 12

// ===== Player ===== //

#define MAX_BREAK_COOLDOWN 0.3f
#define MAX_PLACE_COOLDOWN 0.3f

#define GAMEMODE_CREATIVE 0
#define GAMEMODE_SURVIVAL 1
#define GAMEMODE_SPECTATOR 2

#define DEFAULT_SPEED 4.317f
#define DEFAULT_SNEAK_SPEED 1.295f
#define DEFAULT_SPRINT_SPEED 5.612f

#define WATER_SPEED 1.2f
#define WATER_SNEAK_SPEED 0.9f
#define WATER_SPRINT_SPEED 1.55f

// ===== Camera ===== //

#define DEFAULT_FOV 70.0f
#define DEFAULT_POSITION glm::vec3(0.0f, 0.0f, 0.0f)
#define DEFAULT_EULER_ANGLE glm::vec3(0.0f, 0.0, 0.0f)
#define CAMERA_POSITION_RELATIVE_TO_PLAYER glm::vec3(0.f, 0.75f, 0.f)
#define DELTA_Y_SNEAK 0.1875f

#define DEFAULT_TRANSLATION_SPEED 7.5f
#define DEFAULT_ROTATION_SPEED 0.1f
#define DEFAULT_DISTANCE_SPEED 5.0f
#define KEYS_ROTATION_SPEED_CORRECTION 20.0f

#define DEFAULT_ATTACHED false
#define DEFAULT_MODE 1 // 0 for free camera, 1 for third person camera

// ===== Lights ===== //

#define MAX_LIGHT 15 // Maximum light level
#define MIN_LIGHT 0 // Minimum light level

// ===== Ambiant occlusion ===== //

// For each vertex, the AO is 3 - (side1 + side2 + corner)
// The XYZ deltas are used to calculate the AO for each 4 vertices of a cube face, hence the 4x3 arrays per face

constexpr short AO_South_deltas [4][3] = {
    { -1, -1, -1 },
    {  1, -1, -1 },
    { -1,  1, -1 },
    {  1,  1, -1 },
};

constexpr short AO_North_deltas [4][3] = {
    {  1, -1,  1 },
    { -1, -1,  1 },
    {  1,  1,  1 },
    { -1,  1,  1 },
};

constexpr short AO_East_deltas [4][3] = {
    { -1, -1,  1 },
    { -1, -1, -1 },
    { -1,  1,  1 },
    { -1,  1, -1 },
};

constexpr short AO_West_deltas [4][3] = {
    {  1, -1, -1 },
    {  1, -1,  1 },
    {  1,  1, -1 },
    {  1,  1,  1 }
};

constexpr short AO_Top_deltas [4][3] = {
    { -1,  1, -1 },
    {  1,  1, -1 },
    { -1,  1,  1 },
    {  1,  1,  1 }
};

constexpr short AO_Bottom_deltas [4][3] = {
    {  1, -1,  1 },
    {  1, -1, -1 },
    { -1, -1,  1 },
    { -1, -1, -1 }
};


// ===== FileManager ===== //

const std::string PATH_PLAYER_FILE = "/playerData.bin";
const std::string PATH_SEED = "/seed.bin";
const std::string PATHSAVES = "../saves/";
#define SAVE_DELAY 20 // seconds
const std::string PATH_WORLD_FILE = "/worldData-0-0.bin";

// ===== Menu ===== //
#define MENU_CREATE 1
#define MENU_LOAD 2

// ===== Zombie ===== //
#define ZOMBIE_DISTANCE_FINDING_PLAYER 7.0f
