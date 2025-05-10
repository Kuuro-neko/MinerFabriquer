#pragma once

#include <TP/Scene/SceneNode.hpp>
#include <TP/Scene/BlocTypes.hpp>
#include <common/fastNoiseLit.h>

#include <Defines.hpp>

#include <Exceptions.hpp>


class World;
class ChunkColumn;
struct UngeneratedBlock {
    int x;
    int y;
    int z;
    int chunkX;
    int chunkZ;
    int bloc;
};

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
    bool setBloc(int x, int y, int z, int bloc, bool columnFallback = true, bool worldFallback = true);

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
    bool generationSetBloc(int x, int y, int z, int bloc, bool columnFallback = true, bool worldFallback = true);

    // Allow to save an out of bound bloc to be generated later. Given the coordinates as they are in local with OOB, this functions computes the correction.
    bool saveUngeneratedBlock(int x, int y, int z, int bloc);

    int getBloc(int x, int y, int z, bool columnFallback = true, bool worldFallback = true);
    int getBlocIncludingNeighbors(int x, int y, int z);

    int getLightLevelIncludingNeighbors(int x, int y, int z);

    int getLightLevel(int x, int y, int z);

    int getFaceLight(int x, int y, int z, int face);

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
    int drawOpaque(GLuint programID);
    void drawTransparent(GLuint programID);

    void cleanupBuffers() override;

    bool contains(glm::vec3 point);

    bool contains(Ray ray);

    bool intersects(Ray ray, float maxDistance);

    inline glm::vec3 getChunkCoords() const {
        return glm::vec3(m_chunkCoords.x * m_sizeX, m_chunkCoords.y * m_sizeY, m_chunkCoords.z * m_sizeZ);
    }

    int m_sizeX = CHUNK_SIZE;
    int m_sizeY = CHUNK_SIZE;
    int m_sizeZ = CHUNK_SIZE;
    bool dirty = true;
    std::vector<std::vector<std::vector<int>>> m_cubes;
    std::vector<std::vector<std::vector<int>>> m_lights;
    std::vector<UngeneratedBlock> m_unGeneratedBlocks;
    glm::ivec3 m_chunkCoords;
    World *m_world;
    std::shared_ptr<ChunkColumn> m_chunkColumn;
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

    void addAOValues(int x, int y, int z, unsigned char face, std::vector<float> &ao);

    void addLightValues(int x, int y, int z, unsigned char face, std::vector<int> &lights);
};