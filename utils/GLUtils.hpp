#pragma once

#include <GL/glew.h>
#include <vector>
#include <glm/glm.hpp>

/**
 * @brief Add a square geometry to the mesh object and binds the correct texture to it's vertices
 * 
 * @param vertices 
 * @param triangles 
 * @param uvs 
 * @param bloc You can use a macro such as : AIR, STONE, DIRT, GRASS, PLANKS_OAK, ...
 * @param face defined macro you can use : BLOC_FRONT, BLOC_BACK, BLOC_LEFT, BLOC_RIGHT, BLOC_TOP, BLOC_BOTTOM
 * @param x_offset offset on the x axis relative to the chunk's position
 * @param y_offset offset on the y axis relative to the chunk's position
 * @param z_offset offset on the z axis relative to the chunk's position
 * @param size Default size is 1.0f and should probably not be changed
 */
void addSquareGeometry(std::vector<glm::vec3> &vertices, std::vector<unsigned short> &triangles, std::vector<glm::vec2> &uvs, int bloc, unsigned char face, float x_offset = 0.0f, float y_offset = 0.0f, float z_offset = 0.0f, float size = 1.0f);