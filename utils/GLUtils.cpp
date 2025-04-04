#include <utils/GLUtils.hpp>
#include "GLUtils.hpp"
#include <vector>
#include <glm/glm.hpp>
#include <TP/Scene/BlocTypes.hpp>

#include <TP/Scene/Skybox.hpp>

#define EPSILON 0.001f // To slightly move the texture coordinates to avoid neighboring textures bleeding into each other, doesn't work well

/**
 * @brief Add an offset to the last 4 vertices in the vertices vector
 * 
 * @param vertices 
 * @param x_offset 
 * @param y_offset 
 * @param z_offset 
 */
void addOffsetToLast4Vertices(std::vector<glm::vec3> &vertices, float x_offset, float y_offset, float z_offset) {
    int vertexInsertIndex = vertices.size() - 4;
    for (int i = 0; i < 4; i++) {
        vertices[vertexInsertIndex + i] += glm::vec3(x_offset, y_offset, z_offset);
    }
}

/**
 * @brief Add the quad to the triangles vector
 * 
 * @param triangles 
 * @param vertexInsertIndex 
 */
void addQuadToTriangles(std::vector<unsigned short> &triangles, int vertexInsertIndex) {
    triangles.push_back(vertexInsertIndex);
    triangles.push_back(vertexInsertIndex + 1);
    triangles.push_back(vertexInsertIndex + 2);
    triangles.push_back(vertexInsertIndex + 1);
    triangles.push_back(vertexInsertIndex + 3);
    triangles.push_back(vertexInsertIndex + 2);
}

/**
 * @brief Add the texture coordinates to the uvs vector
 * 
 * @param uvs 
 * @param texCoords 
 */
void addUvs(std::vector<glm::vec2> &uvs, std::pair<float, float> texCoords) {
    uvs.push_back(glm::vec2(texCoords.first + EPSILON, texCoords.second + TEXTUREATLAS_UNIT - EPSILON));
    uvs.push_back(glm::vec2(texCoords.first + TEXTUREATLAS_UNIT - EPSILON, texCoords.second + TEXTUREATLAS_UNIT - EPSILON));
    uvs.push_back(glm::vec2(texCoords.first + EPSILON, texCoords.second + EPSILON));
    uvs.push_back(glm::vec2(texCoords.first + TEXTUREATLAS_UNIT - EPSILON, texCoords.second + EPSILON));
}

void addSquareGeometry(std::vector<glm::vec3> &vertices, std::vector<unsigned short> &triangles, std::vector<glm::vec2> &uvs, int bloc, unsigned char face, float x_offset, float y_offset, float z_offset, float size) {
    int vertexInsertIndex = vertices.size();
    BlocDatabase &blocDatabase = BlocDatabase::getInstance();
    if (face & BLOC_FRONT) {
        vertices.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
        vertices.push_back(glm::vec3(size, 0.0f, 0.0f));
        vertices.push_back(glm::vec3(0.0f, size, 0.0f));
        vertices.push_back(glm::vec3(size, size, 0.0f));

        addOffsetToLast4Vertices(vertices, x_offset, y_offset, z_offset);
        addQuadToTriangles(triangles, vertexInsertIndex);
        
        addUvs(uvs, blocDatabase.getTexCoords(bloc, BLOC_FRONT));
    }
    
    if (face & BLOC_BACK) {
        vertices.push_back(glm::vec3(size, 0.0f, size));
        vertices.push_back(glm::vec3(0.0f, 0.0f, size));
        vertices.push_back(glm::vec3(size, size, size));
        vertices.push_back(glm::vec3(0.0f, size, size));

        addOffsetToLast4Vertices(vertices, x_offset, y_offset, z_offset);
        addQuadToTriangles(triangles, vertexInsertIndex);
        addUvs(uvs, blocDatabase.getTexCoords(bloc, BLOC_BACK));
    }
    
    if (face & BLOC_LEFT) {
        vertices.push_back(glm::vec3(0.0f, 0.0f, size));
        vertices.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
        vertices.push_back(glm::vec3(0.0f, size, size));
        vertices.push_back(glm::vec3(0.0f, size, 0.0f));

        addOffsetToLast4Vertices(vertices, x_offset, y_offset, z_offset);
        addQuadToTriangles(triangles, vertexInsertIndex);
        addUvs(uvs, blocDatabase.getTexCoords(bloc, BLOC_LEFT));
    }
    
    if (face & BLOC_RIGHT) {
        vertices.push_back(glm::vec3(size, 0.0f, 0.0f));
        vertices.push_back(glm::vec3(size, 0.0f, size));
        vertices.push_back(glm::vec3(size, size, 0.0f));
        vertices.push_back(glm::vec3(size, size, size));

        addOffsetToLast4Vertices(vertices, x_offset, y_offset, z_offset);
        addQuadToTriangles(triangles, vertexInsertIndex);
        addUvs(uvs, blocDatabase.getTexCoords(bloc, BLOC_RIGHT));
    }
    if (face & BLOC_TOP) {
        vertices.push_back(glm::vec3(0.0f, size, 0.0f));
        vertices.push_back(glm::vec3(size, size, 0.0f));
        vertices.push_back(glm::vec3(0.0f, size, size));
        vertices.push_back(glm::vec3(size, size, size));

        addOffsetToLast4Vertices(vertices, x_offset, y_offset, z_offset);
        addQuadToTriangles(triangles, vertexInsertIndex);
        addUvs(uvs, blocDatabase.getTexCoords(bloc, BLOC_TOP));
    }
    if (face & BLOC_BOTTOM) {
        vertices.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
        vertices.push_back(glm::vec3(size, 0.0f, 0.0f));
        vertices.push_back(glm::vec3(0.0f, 0.0f, size));
        vertices.push_back(glm::vec3(size, 0.0f, size));

        addOffsetToLast4Vertices(vertices, x_offset, y_offset, z_offset);
        addQuadToTriangles(triangles, vertexInsertIndex);
        addUvs(uvs, blocDatabase.getTexCoords(bloc, BLOC_BOTTOM));
    }
}

void addSkyboxUvs(std::vector<glm::vec2> &uvs, SkyboxTexCoords &texCoords) {
    uvs.push_back(glm::vec2(texCoords.front_x + EPSILON, texCoords.front_y + texCoords.unit_y - EPSILON));
    uvs.push_back(glm::vec2(texCoords.front_x + texCoords.unit_x - EPSILON, texCoords.front_y + texCoords.unit_y - EPSILON));
    uvs.push_back(glm::vec2(texCoords.front_x + EPSILON, texCoords.front_y + EPSILON));
    uvs.push_back(glm::vec2(texCoords.front_x + texCoords.unit_x - EPSILON, texCoords.front_y + EPSILON));
}

void addSkyboxGeometry(std::vector<glm::vec3> &vertices, std::vector<unsigned short> &triangles, std::vector<glm::vec2> &uvs, float size) {
    int vertexInsertIndex = vertices.size();
    SkyboxTexCoords texCoords = SkyboxTexCoords();    
    vertices.push_back(glm::vec3(-size, -size, -size));
    vertices.push_back(glm::vec3(size, -size, -size));
    vertices.push_back(glm::vec3(-size, size, -size));
    vertices.push_back(glm::vec3(size, size, -size));
    addQuadToTriangles(triangles, vertexInsertIndex);
    addSkyboxUvs(uvs, texCoords);
    

    vertices.push_back(glm::vec3(size, -size, size));
    vertices.push_back(glm::vec3(-size, -size, size));
    vertices.push_back(glm::vec3(size, size, size));
    vertices.push_back(glm::vec3(-size, size, size));
    addQuadToTriangles(triangles, vertexInsertIndex);
    addSkyboxUvs(uvs, texCoords);

    vertices.push_back(glm::vec3(-size, -size, size));
    vertices.push_back(glm::vec3(-size, -size, -size));
    vertices.push_back(glm::vec3(-size, size, size));
    vertices.push_back(glm::vec3(-size, size, -size));

    addQuadToTriangles(triangles, vertexInsertIndex);
    addSkyboxUvs(uvs, texCoords);
    vertices.push_back(glm::vec3(size, -size, -size));
    vertices.push_back(glm::vec3(size, -size, size));
    vertices.push_back(glm::vec3(size, size, -size));
    vertices.push_back(glm::vec3(size, size, size));
    addQuadToTriangles(triangles, vertexInsertIndex);
    addSkyboxUvs(uvs, texCoords);

    vertices.push_back(glm::vec3(-size, size, -size));
    vertices.push_back(glm::vec3(size, size, -size));
    vertices.push_back(glm::vec3(-size, size, size));
    vertices.push_back(glm::vec3(size, size, size));
    addQuadToTriangles(triangles, vertexInsertIndex);
    addSkyboxUvs(uvs, texCoords);

    vertices.push_back(glm::vec3(-size, -size, -size));
    vertices.push_back(glm::vec3(size, -size, -size));
    vertices.push_back(glm::vec3(-size, -size, size));
    vertices.push_back(glm::vec3(size, -size, size));
    addQuadToTriangles(triangles, vertexInsertIndex);
    addSkyboxUvs(uvs, texCoords);
}