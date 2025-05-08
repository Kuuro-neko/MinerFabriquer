#pragma once
#include <string>
// ===== Maths ===== //

#define EPSILON 0.001f // To slightly move the texture coordinates to avoid neighboring textures bleeding into each other, doesn't work well

// ===== Generation & chunks ===== //

#define GENERATION_SEED 110111
#define GROUND_LEVEL 65.0f
#define WATER_LEVEL 63

#define CHUNK_SIZE 16
#define BLOC_SIZE 1

#define OUT_OF_BOUNDS_BLOC -2

#define GENERATION_RADIUS_X 8 // X radius of initial generation
#define GENERATION_SIZE_Y 8 // Y height of initial generation
#define GENERATION_RADIUS_Z 8 // Z radius of initial generation


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

#define RENDERER_DISTANCE 32.0f

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
const std::string PATHSAVES = "../saves/";
#define SAVE_DELAY 50 // seconds
