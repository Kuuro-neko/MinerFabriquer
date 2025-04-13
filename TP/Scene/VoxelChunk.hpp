#pragma once

#include <TP/Scene/SceneNode.hpp>
#include <TP/Scene/BlocTypes.hpp>
#include <common/fastNoiseLit.h>
#define DEFAULT_CHUNK_SIZE 16
#define DEFAULT_CHUNK_HEIGHT 128

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
    bool setBloc(int x, int y, int z, int bloc, bool genMesh=true);

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

    int playerRemoveBlock(int x, int y, int z);

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

    void cleanupBuffers() override;

    bool contains(glm::vec3 point);

    bool contains(Ray ray);

    bool intersects(Ray ray, float maxDistance);

    int m_sizeX = DEFAULT_CHUNK_SIZE;
    int m_sizeY = DEFAULT_CHUNK_SIZE;
    int m_sizeZ = DEFAULT_CHUNK_HEIGHT;
    int*** m_cubes;
    glm::ivec3 m_chunkCoords;
    World *m_world;

    // Move Constructor
    VoxelChunk(VoxelChunk&& other) noexcept;

    // Move Assignment Operator
    VoxelChunk& operator=(VoxelChunk&& other) noexcept;

    // Copy Constructor
    VoxelChunk(const VoxelChunk& other);

    // Copy Assignment Operator
    VoxelChunk& operator=(const VoxelChunk& other);


private:
    void allocateCubes();

    void cleanup();

};