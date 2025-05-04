#include <utils/GLUtils.hpp>
#include "GLUtils.hpp"
#include <vector>
#include <glm/glm.hpp>
#include <TP/Scene/BlocTypes.hpp>
#include <Defines.hpp>

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
    triangles.push_back(vertexInsertIndex + 2);
    triangles.push_back(vertexInsertIndex + 1);
    triangles.push_back(vertexInsertIndex + 1);
    triangles.push_back(vertexInsertIndex + 2);
    triangles.push_back(vertexInsertIndex + 3);
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

void addNormals(std::vector<glm::vec3> &normals, unsigned char face) {
    glm::vec3 normal;
    if (face & FACE_SOUTH) {
        normal = glm::vec3(0.0f, 0.0f, -1.0f);
    } else if (face & FACE_NORTH) {
        normal = glm::vec3(0.0f, 0.0f, 1.0f);
    } else if (face & FACE_EAST) {
        normal = glm::vec3(-1.0f, 0.0f, 0.0f);
    } else if (face & FACE_WEST) {
        normal = glm::vec3(1.0f, 0.0f, 0.0f);
    } else if (face & FACE_TOP) {
        normal = glm::vec3(0.0f, 1.0f, 0.0f);
    } else if (face & FACE_BOTTOM) {
        normal = glm::vec3(0.0f, -1.0f, 0.0f);
    }
    normals.push_back(normal);
    normals.push_back(normal);
    normals.push_back(normal);
    normals.push_back(normal);
}

void addSquareGeometry(VoxelMeshObject &mesh, int bloc, unsigned char face, float x_offset, float y_offset, float z_offset, bool lowerTop, float size) {
    int vertexInsertIndex = mesh.vertices.size();
    BlocDatabase &blocDatabase = BlocDatabase::getInstance();
    if (face & FACE_SOUTH) {
        mesh.vertices.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
        mesh.vertices.push_back(glm::vec3(size, 0.0f, 0.0f));
        mesh.vertices.push_back(glm::vec3(0.0f, size, 0.0f));
        mesh.vertices.push_back(glm::vec3(size, size, 0.0f));

        addOffsetToLast4Vertices(mesh.vertices, x_offset, y_offset, z_offset);
        addQuadToTriangles(mesh.triangles, vertexInsertIndex);
        
        addUvs(mesh.uvs, blocDatabase.getTexCoords(bloc, FACE_SOUTH));
        addNormals(mesh.normals, FACE_SOUTH);
    }
    
    if (face & FACE_NORTH) {
        mesh.vertices.push_back(glm::vec3(size, 0.0f, size));
        mesh.vertices.push_back(glm::vec3(0.0f, 0.0f, size));
        mesh.vertices.push_back(glm::vec3(size, size, size));
        mesh.vertices.push_back(glm::vec3(0.0f, size, size));

        addOffsetToLast4Vertices(mesh.vertices, x_offset, y_offset, z_offset);
        addQuadToTriangles(mesh.triangles, vertexInsertIndex);
        addUvs(mesh.uvs, blocDatabase.getTexCoords(bloc, FACE_NORTH));
        addNormals(mesh.normals, FACE_NORTH);
    }
    
    if (face & FACE_EAST) {
        mesh.vertices.push_back(glm::vec3(0.0f, 0.0f, size));
        mesh.vertices.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
        mesh.vertices.push_back(glm::vec3(0.0f, size, size));
        mesh.vertices.push_back(glm::vec3(0.0f, size, 0.0f));

        addOffsetToLast4Vertices(mesh.vertices, x_offset, y_offset, z_offset);
        addQuadToTriangles(mesh.triangles, vertexInsertIndex);
        addUvs(mesh.uvs, blocDatabase.getTexCoords(bloc, FACE_EAST));
        addNormals(mesh.normals, FACE_EAST);
    }
    
    if (face & FACE_WEST) {
        mesh.vertices.push_back(glm::vec3(size, 0.0f, 0.0f));
        mesh.vertices.push_back(glm::vec3(size, 0.0f, size));
        mesh.vertices.push_back(glm::vec3(size, size, 0.0f));
        mesh.vertices.push_back(glm::vec3(size, size, size));

        addOffsetToLast4Vertices(mesh.vertices, x_offset, y_offset, z_offset);
        addQuadToTriangles(mesh.triangles, vertexInsertIndex);
        addUvs(mesh.uvs, blocDatabase.getTexCoords(bloc, FACE_WEST));
        addNormals(mesh.normals, FACE_WEST);
    }
    if (face & FACE_TOP) {
        float sizeY = size;
        if (lowerTop) {
            sizeY *= 0.875f;
        }
        mesh.vertices.push_back(glm::vec3(0.0f, sizeY, 0.0f));
        mesh.vertices.push_back(glm::vec3(size, sizeY, 0.0f));
        mesh.vertices.push_back(glm::vec3(0.0f, sizeY, size));
        mesh.vertices.push_back(glm::vec3(size, sizeY, size));

        addOffsetToLast4Vertices(mesh.vertices, x_offset, y_offset, z_offset);
        addQuadToTriangles(mesh.triangles, vertexInsertIndex);
        addUvs(mesh.uvs, blocDatabase.getTexCoords(bloc, FACE_TOP));
        addNormals(mesh.normals, FACE_TOP);
    }
    if (face & FACE_BOTTOM) {
        mesh.vertices.push_back(glm::vec3(size, 0.0f, size));
        mesh.vertices.push_back(glm::vec3(size, 0.0f, 0.0f));
        mesh.vertices.push_back(glm::vec3(0.0f, 0.0f, size));
        mesh.vertices.push_back(glm::vec3(0.0f, 0.0f, 0.0f));

        addOffsetToLast4Vertices(mesh.vertices, x_offset, y_offset, z_offset);
        addQuadToTriangles(mesh.triangles, vertexInsertIndex);
        addUvs(mesh.uvs, blocDatabase.getTexCoords(bloc, FACE_BOTTOM));
        addNormals(mesh.normals, FACE_BOTTOM);
    }
}

