#pragma once

#include <TP/Scene/SceneNode.hpp>
#include <TP/Scene/BlocTypes.hpp>
#include <common/fastNoiseLit.h>
#define DEFAULT_CHUNK_SIZE 16
#define DEFAULT_CHUNK_HEIGHT 128

#define MAX_LIGHT 15
#define MIN_LIGHT 0

class World;

class VoxelChunk : public SceneNode
{
public:
    VoxelChunk(int sizeX, int sizeY, int sizeZ);
    VoxelChunk();
    ~VoxelChunk();
    /**
     * @brief Set a block in the chunk, used for gameplay
     * 
     * @param x 
     * @param y 
     * @param z 
     * @param bloc 
     * @param genMesh 
     * @return true 
     * @return false 
     */
    bool setBloc(int x, int y, int z, int bloc);

    /**
     * @brief Set a block in the chunk, used for generation so it bypasses some checks
     * 
     * @param x 
     * @param y 
     * @param z 
     * @param bloc 
     * @return true 
     * @return false 
     */
    bool generationSetBloc(int x, int y, int z, int bloc);

    int getBloc(int x, int y, int z);
    int getBlocIncludingNeighbors(int x, int y, int z);

    unsigned short getLightLevelIncludingNeighbors(int x, int y, int z);

    unsigned short getFaceLight(int x, int y, int z, int face);

    int playerRemoveBlock(int x, int y, int z, unsigned char gamemode);

    int removeBlock(int x, int y, int z);

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
    void drawTransparent(GLuint programID);

    void cleanupBuffers() override;

    bool contains(glm::vec3 point);

    bool contains(Ray ray);

    bool intersects(Ray ray, float maxDistance);

    inline glm::vec3 getChunkCoords() const {
        return glm::vec3(m_chunkCoords.x * m_sizeX, m_chunkCoords.y * m_sizeY, m_chunkCoords.z * m_sizeZ);
    }

    int m_sizeX = DEFAULT_CHUNK_SIZE;
    int m_sizeY = DEFAULT_CHUNK_SIZE;
    int m_sizeZ = DEFAULT_CHUNK_HEIGHT;
    bool dirty = true;
    std::vector<std::vector<std::vector<int>>> m_cubes;
    std::vector<std::vector<std::vector<int>>> m_lights;
    glm::ivec3 m_chunkCoords;
    World *m_world;
    VoxelMeshObject m_opaqueMesh;
    VoxelMeshObject m_transparentMesh;

    // Move Constructor
    VoxelChunk(VoxelChunk&& other) noexcept;

    // Move Assignment Operator
    VoxelChunk& operator=(VoxelChunk&& other) noexcept;

    // Copy Constructor
    VoxelChunk(const VoxelChunk& other) = delete;

    // Copy Assignment Operator
    VoxelChunk& operator=(const VoxelChunk& other) = delete;



private:
    void allocateCubes();

    void cleanup();

    void markDirtyNeighbors(int x, int y, int z);
};