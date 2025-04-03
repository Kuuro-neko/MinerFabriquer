#pragma once

#include <GL/glew.h>
#include <vector>
#include <glm/glm.hpp>

void addSquareGeometry(std::vector<glm::vec3> &vertices, std::vector<unsigned short> &triangles, std::vector<glm::vec2> &uvs, int bloc, unsigned char face, float x_offset = 0.0f, float y_offset = 0.0f, float z_offset = 0.0f, float size = 1.0f);