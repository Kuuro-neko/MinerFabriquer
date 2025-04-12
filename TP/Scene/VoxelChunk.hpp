#pragma once

#include <TP/Scene/SceneNode.hpp>
#include <TP/Scene/BlocTypes.hpp>
#include <common/fastNoiseLit.h>
#define DEFAULT_CHUNK_SIZE 16
#define DEFAULT_CHUNK_HEIGHT 128

class VoxelChunk : public SceneNode
{
public:
    VoxelChunk(int sizeX, int sizeY, int sizeZ) : SceneNode(Transform(), new MeshObject(), nullptr), m_sizeX(sizeX), m_sizeY(sizeY), m_sizeZ(sizeZ) {
        allocateCubes();
    }
    VoxelChunk() : VoxelChunk(DEFAULT_CHUNK_SIZE, DEFAULT_CHUNK_SIZE, DEFAULT_CHUNK_HEIGHT) {}
    ~VoxelChunk() {
        cleanup();
    }
    bool setBloc(int x, int y, int z, int bloc) {
        //std::cout << "Setting " << BlocDatabase::getInstance().getBloc(bloc)->name << " at " << x << ", " << y << ", " << z << std::endl;
        if (x < 0 || x >= m_sizeX || y < 0 || y >= m_sizeY || z < 0 || z >= m_sizeZ) {
            //std::cout << "Error: Out of bounds" << std::endl;
            return false;
        }
        if (m_cubes[x][y][z] != AIR) {
            //std::cout << "Error: Block already set" << std::endl;
            return false;
        }
        m_cubes[x][y][z] = bloc;
        generateMesh();
        return true;
    }

    int getBloc(int x, int y, int z) {
        return m_cubes[x][y][z];
    }

    int playerRemoveBlock(int x, int y, int z) {
        if (x < 0 || x >= m_sizeX || y < 0 || y >= m_sizeY || z < 0 || z >= m_sizeZ) {
            std::cout << "Error: Out of bounds" << std::endl;
            return -1;
        }
        if (BlocDatabase::getInstance().isAir(m_cubes[x][y][z])) {
            std::cout << "Error: Cannot remove air block" << std::endl;
            return -1;
        }
        if (BlocDatabase::getInstance().isUnbreakable(m_cubes[x][y][z])) {
            std::cout << "Error: Cannot remove unbreakable block" << std::endl;
            return -1;
        }
        return removeBlock(x, y, z);
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

    bool contains(glm::vec3 point) {
        return point.x >= getWorldPosition().x && point.x <= getWorldPosition().x + m_sizeX &&
               point.y >= getWorldPosition().y && point.y <= getWorldPosition().y + m_sizeY &&
               point.z >= getWorldPosition().z && point.z <= getWorldPosition().z + m_sizeZ;
    }

    bool contains(Ray ray) {
        return contains(ray.origin) || contains(ray.origin + ray.direction);
    }

    bool intersects(Ray ray, float maxDistance) {
        return contains(ray) || ray.rayIntersectsAABB(ray, getWorldPosition(), getWorldPosition() + glm::vec3(m_sizeX, m_sizeY, m_sizeZ), maxDistance);
    }

    int m_sizeX;
    int m_sizeY;
    int m_sizeZ;
    int*** m_cubes;

    // Move Constructor
    VoxelChunk(VoxelChunk&& other) noexcept
        : SceneNode(std::move(other)), m_sizeX(other.m_sizeX), m_sizeY(other.m_sizeY), m_sizeZ(other.m_sizeZ), m_cubes(other.m_cubes) {
        other.m_cubes = nullptr; // Nullify the source pointer
    }

    // Move Assignment Operator
    VoxelChunk& operator=(VoxelChunk&& other) noexcept {
        if (this != &other) {
            cleanup(); // Free existing resources
            SceneNode::operator=(std::move(other));
            m_sizeX = other.m_sizeX;
            m_sizeY = other.m_sizeY;
            m_sizeZ = other.m_sizeZ;
            m_cubes = other.m_cubes;
            other.m_cubes = nullptr; // Nullify the source pointer
        }
        return *this;
    }

    // Copy Constructor
    VoxelChunk(const VoxelChunk& other)
        : SceneNode(other), m_sizeX(other.m_sizeX), m_sizeY(other.m_sizeY), m_sizeZ(other.m_sizeZ) {
        allocateCubes();
        for (int x = 0; x < m_sizeX; ++x) {
            for (int y = 0; y < m_sizeY; ++y) {
                for (int z = 0; z < m_sizeZ; ++z) {
                    m_cubes[x][y][z] = other.m_cubes[x][y][z];
                }
            }
        }
    }

    // Copy Assignment Operator
    VoxelChunk& operator=(const VoxelChunk& other) {
        if (this != &other) {
            cleanup(); // Free existing resources
            SceneNode::operator=(other);
            m_sizeX = other.m_sizeX;
            m_sizeY = other.m_sizeY;
            m_sizeZ = other.m_sizeZ;
            allocateCubes();
            for (int x = 0; x < m_sizeX; ++x) {
                for (int y = 0; y < m_sizeY; ++y) {
                    for (int z = 0; z < m_sizeZ; ++z) {
                        m_cubes[x][y][z] = other.m_cubes[x][y][z];
                    }
                }
            }
        }
        return *this;
    }

    void drawGPU(GLuint programID);

private:
    void allocateCubes() {
        m_cubes = new int**[m_sizeX];
        for (int i = 0; i < m_sizeX; ++i) {
            m_cubes[i] = new int*[m_sizeY];
            for (int j = 0; j < m_sizeY; ++j) {
                m_cubes[i][j] = new int[m_sizeZ];
                for (int k = 0; k < m_sizeZ; ++k) {
                    m_cubes[i][j][k] = AIR;
                }
            }
        }
    }

    void cleanup() {
        if (m_cubes) {
            for (int i = 0; i < m_sizeX; ++i) {
                for (int j = 0; j < m_sizeY; ++j) {
                    delete[] m_cubes[i][j];
                }
                delete[] m_cubes[i];
            }
            delete[] m_cubes;
            m_cubes = nullptr;
        }
        cleanupBuffers();
    }

};