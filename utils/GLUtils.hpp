#pragma once

#include <GL/glew.h>
#include <vector>
#include <glm/glm.hpp>

#include <TP/Scene/MeshObject.hpp>

/**
 * @brief Add a square geometry to the mesh object and binds the correct texture to it's vertices
 * 
 * @param mesh The chunk's mesh object
 * @param bloc You can use a macro such as : AIR, STONE, DIRT, GRASS, PLANKS_OAK, ...
 * @param face defined macro you can use : FACE_SOUTH, FACE_NORTH, FACE_EAST, FACE_WEST, FACE_TOP, FACE_BOTTOM
 * @param x_offset offset on the x axis relative to the chunk's position
 * @param y_offset offset on the y axis relative to the chunk's position
 * @param z_offset offset on the z axis relative to the chunk's position
 * @param size Default size is 1.0f and should probably not be changed
 */
void addSquareGeometry(std::shared_ptr<VoxelMeshObject> mesh, int bloc, unsigned char face, float x_offset = 0.0f, float y_offset = 0.0f, float z_offset = 0.0f, bool lowerTop = false, float size = 1.0f);