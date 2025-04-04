#pragma once

#include <TP/Scene/SceneNode.hpp>
#include <TP/Scene/BlocTypes.hpp>

class VoxelChunk : public SceneNode
{
public:
    VoxelChunk(int sizeX, int sizeY, int sizeZ, Transform t, MeshObject* m) : SceneNode(t, m, nullptr), m_sizeX(sizeX), m_sizeY(sizeY), m_sizeZ(sizeZ) {
        m_cubes = new int**[m_sizeX];
        for (int i = 0; i < m_sizeX; i++) {
            m_cubes[i] = new int*[m_sizeY];
            for (int j = 0; j < m_sizeY; j++) {
                m_cubes[i][j] = new int[m_sizeZ];
                for (int k = 0; k < m_sizeZ; k++) {
                    m_cubes[i][j][k] = AIR;
                }
            }
        }
    }
    ~VoxelChunk() {
        for (int i = 0; i < m_sizeX; i++) {
            for (int j = 0; j < m_sizeY; j++) {
                delete[] m_cubes[i][j];
            }
            delete[] m_cubes[i];
        }
        delete[] m_cubes;
    }
    void setBloc(int x, int y, int z, int bloc) {
        m_cubes[x][y][z] = bloc;
    }
    int getBloc(int x, int y, int z) {
        return m_cubes[x][y][z];
    }

    int removeBlock(int x, int y, int z) {
        int id = m_cubes[x][y][z];
        m_cubes[x][y][z] = AIR;
        generateMesh();
        return id;
    }

    /**
     * @brief Generate the mesh for the chunk, need to be called after setting the blocks and editing the chunk during gameplay
     * 
     */
    void generateMesh();

    /**
     * @brief Draw the chunk
     * 
     * @param programID 
     */
    void draw(GLuint programID) override;

    void cleanupBuffers() override {
        m_mesh->cleanupBuffers();
    }

    int m_sizeX;
    int m_sizeY;
    int m_sizeZ;
    int*** m_cubes;
};