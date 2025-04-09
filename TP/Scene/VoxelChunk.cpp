#include <TP/Scene/VoxelChunk.hpp>

void VoxelChunk::generateMesh() {
    m_mesh->vertices.clear();
    m_mesh->triangles.clear();
    m_mesh->uvs.clear();

    // Loop through all the cubes in the chunk. If a cube has a face that is not adjacent to another non opaque cube, add a face to the mesh
    for (int x = 0; x < m_sizeX; x++) {
        for (int y = 0; y < m_sizeY; y++) {
            for (int z = 0; z < m_sizeZ; z++) {
                if (m_cubes[x][y][z] != AIR) {
                    // Check all the adjacent cubes to see if they are air or leaves
                    if (x == 0 || m_cubes[x - 1][y][z] == AIR || m_cubes[x - 1][y][z] == LEAVES_OAK) {
                        addSquareGeometry(m_mesh->vertices, m_mesh->triangles, m_mesh->uvs, m_cubes[x][y][z], BLOC_LEFT, x, y, z);
                    }
                    if (x == m_sizeX - 1 || m_cubes[x + 1][y][z] == AIR || m_cubes[x + 1][y][z] == LEAVES_OAK) {
                        addSquareGeometry(m_mesh->vertices, m_mesh->triangles, m_mesh->uvs, m_cubes[x][y][z], BLOC_RIGHT, x, y, z);
                    }
                    if (y == 0 || m_cubes[x][y - 1][z] == AIR || m_cubes[x][y - 1][z] == LEAVES_OAK) {
                        addSquareGeometry(m_mesh->vertices, m_mesh->triangles, m_mesh->uvs, m_cubes[x][y][z], BLOC_BOTTOM, x, y, z);
                    }
                    if (y == m_sizeY - 1 || m_cubes[x][y + 1][z] == AIR || m_cubes[x][y + 1][z] == LEAVES_OAK) {
                        addSquareGeometry(m_mesh->vertices, m_mesh->triangles, m_mesh->uvs, m_cubes[x][y][z], BLOC_TOP, x, y, z);
                    }
                    if (z == 0 || m_cubes[x][y][z - 1] == AIR || m_cubes[x][y][z - 1] == LEAVES_OAK) {
                        addSquareGeometry(m_mesh->vertices, m_mesh->triangles, m_mesh->uvs, m_cubes[x][y][z], BLOC_FRONT, x, y, z);
                    }
                    if (z == m_sizeZ - 1 || m_cubes[x][y][z + 1] == AIR || m_cubes[x][y][z + 1] == LEAVES_OAK) {
                        addSquareGeometry(m_mesh->vertices, m_mesh->triangles, m_mesh->uvs, m_cubes[x][y][z], BLOC_BACK, x, y, z);
                    }
                }
            }
        }
    }
    m_mesh->initializeBuffers();
}

void VoxelChunk::draw(GLuint programID) {
    GLuint modelMatrixId = glGetUniformLocation(programID, "ModelMatrix");
    glUniformMatrix4fv(modelMatrixId, 1, false, &ModelMatrix[0][0]);
    
    TextureAtlas::getInstance().bind(programID);
    m_mesh->draw(programID);
}