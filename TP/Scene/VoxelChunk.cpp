#include <TP/Scene/VoxelChunk.hpp>
#include <TP/Scene/World.hpp>
#include <TP/Character/Character.hpp>
#include "VoxelChunk.hpp"

VoxelChunk::VoxelChunk(int sizeX, int sizeY, int sizeZ) : SceneNode(Transform(), nullptr, nullptr), m_sizeX(sizeX), m_sizeY(sizeY), m_sizeZ(sizeZ) {
    allocateCubes();
    m_opaqueMesh = VoxelMeshObject();
    m_transparentMesh = VoxelMeshObject();
}
VoxelChunk::VoxelChunk() : VoxelChunk(CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE) {
    allocateCubes();
}
VoxelChunk::~VoxelChunk() {
    cleanup();
}

bool VoxelChunk::setBloc(int x, int y, int z, int bloc) {
    //std::cout << "Setting " << BlocDatabase::getInstance().getBloc(bloc)->name << " at " << x << ", " << y << ", " << z << std::endl;
    if (x < 0 || x >= m_sizeX || y < 0 || y >= m_sizeY || z < 0 || z >= m_sizeZ) {
        //std::cout << "Error: Out of bounds" << std::endl;
        return m_world->setBloc(x + m_chunkCoords.x * m_sizeX, y + m_chunkCoords.y * m_sizeY, z + m_chunkCoords.z * m_sizeZ, bloc);
    }
    if (m_cubes[x][y][z] != AIR) {
        //std::cout << "Error: Block already set" << std::endl;
        return false;
    }
    m_cubes[x][y][z] = bloc;
    m_lights[x][y][z] = BlocDatabase::getInstance().defaultLightLevel(bloc);
    dirty = true;
    markDirtyNeighbors(x, y, z);
    return true;
}

bool VoxelChunk::generationSetBloc(int x, int y, int z, int bloc) {
    if (y < 0 || y >= m_sizeY) {
        return m_world->generationSetBloc(x + m_chunkCoords.x * m_sizeX, y + m_chunkCoords.y * m_sizeY, z + m_chunkCoords.z * m_sizeZ, bloc);
    }
    if (x < 0) {
        m_unGeneratedBlocks.push_back({betterModulo(x, CHUNK_SIZE), y, z, m_chunkCoords.x -1, m_chunkCoords.z, bloc});
        return false;
    }
    if (x >= m_sizeX) {
        m_unGeneratedBlocks.push_back({betterModulo(x, CHUNK_SIZE), y, z, m_chunkCoords.x +1, m_chunkCoords.z, bloc});
        return false;
    }
    if (z < 0) {
        m_unGeneratedBlocks.push_back({x, y, betterModulo(z, CHUNK_SIZE), m_chunkCoords.x, m_chunkCoords.z -1, bloc});
        return false;
    }
    if (z >= m_sizeZ) {
        m_unGeneratedBlocks.push_back({x, y, betterModulo(z, CHUNK_SIZE), m_chunkCoords.x, m_chunkCoords.z +1, bloc});
        return false;
    }
    m_cubes[x][y][z] = bloc;
    m_lights[x][y][z] = BlocDatabase::getInstance().defaultLightLevel(bloc);
    dirty = true;
    return true;
}

int VoxelChunk::getBloc(int x, int y, int z) {
    //quand on sort du chunk, on renvoie -1
    if (x < 0 || x >= m_sizeX || y < 0 || y >= m_sizeY || z < 0 || z >= m_sizeZ) {
        return m_world->getBloc(x + m_chunkCoords.x * m_sizeX, y + m_chunkCoords.y * m_sizeY, z + m_chunkCoords.z * m_sizeZ);
    }
    return m_cubes[x][y][z];
}

int VoxelChunk::getBlocIncludingNeighbors(int x, int y, int z) {
    if (x < 0 || x >= m_sizeX || y < 0 || y >= m_sizeY || z < 0 || z >= m_sizeZ) {
        int a = m_world->getBloc(x + m_chunkCoords.x * m_sizeX, y + m_chunkCoords.y * m_sizeY, z + m_chunkCoords.z * m_sizeZ);
        return a;
    }
    return m_cubes[x][y][z];
}

unsigned short VoxelChunk::getLightLevelIncludingNeighbors(int x, int y, int z) {
    if (x < 0 || x >= m_sizeX || y < 0 || y >= m_sizeY || z < 0 || z >= m_sizeZ) {
        int a = m_world->getLightLevel(x + m_chunkCoords.x * m_sizeX, y + m_chunkCoords.y * m_sizeY, z + m_chunkCoords.z * m_sizeZ);
        return a;
    }
    return m_lights[x][y][z];
}

int VoxelChunk::playerRemoveBlock(int x, int y, int z, unsigned char gamemode) {
    if (x < 0 || x >= m_sizeX || y < 0 || y >= m_sizeY || z < 0 || z >= m_sizeZ) {
        std::cout << "Error: Out of bounds" << std::endl;
        return -1;
    }
    if (BlocDatabase::getInstance().isAir(m_cubes[x][y][z])) {
        std::cout << "Error: Cannot remove air block" << std::endl;
        return -1;
    }
    if (BlocDatabase::getInstance().isUnbreakable(m_cubes[x][y][z]) && gamemode != GAMEMODE_CREATIVE) {
        std::cout << "Error: Cannot remove unbreakable block" << std::endl;
        return -1;
    }
    return removeBlock(x, y, z);
}

int VoxelChunk::removeBlock(int x, int y, int z) {
    int id = m_cubes[x][y][z];
    m_cubes[x][y][z] = AIR;
    m_lights[x][y][z] = MIN_LIGHT;
    dirty = true;
    markDirtyNeighbors(x, y, z);
    return id;
}

bool opaqueNeighborCheck(int neighbor) {
    return neighbor == AIR || !BlocDatabase::getInstance().isOpaque(neighbor); // || neighbor == OUT_OF_BOUNDS_BLOC; // to display chunk sides even if it's out of bounds
}

bool transparentNeighborCheck(int neighbor, int current, unsigned char face) {
    return neighbor == AIR || (current == WATER && face & FACE_TOP && BlocDatabase::getInstance().isSolid(neighbor)); //  || neighbor == OUT_OF_BOUNDS_BLOC;
}



unsigned short VoxelChunk::getFaceLight(int x, int y, int z, int face) {
    unsigned short light = 0;
    if (face & FACE_EAST) {
        light = this->getLightLevelIncludingNeighbors(x - 1, y, z);
    }
    if (face & FACE_WEST) {
        light = this->getLightLevelIncludingNeighbors(x + 1, y, z);
    }
    if (face & FACE_BOTTOM) {
        light = this->getLightLevelIncludingNeighbors(x, y - 1, z);
    }
    if (face & FACE_TOP) {
        light = this->getLightLevelIncludingNeighbors(x, y + 1, z);
    }
    if (face & FACE_SOUTH) {
        light = this->getLightLevelIncludingNeighbors(x, y, z - 1);
    }
    if (face & FACE_NORTH) {
        light = this->getLightLevelIncludingNeighbors(x, y, z + 1);
    }
    return light;
}

void VoxelChunk::addAOValues(int x, int y, int z, unsigned char face, std::vector<float> &ao) {
    if (!BlocDatabase::getInstance().isSolid(m_cubes[x][y][z])) {
        for(int i = 0; i < 4; i++) ao.push_back(3);
        return;
    }
    const short (*AO_deltas)[3] = nullptr;
    int corner, left, right;
    switch(face) {
        case FACE_EAST:
            AO_deltas = AO_East_deltas;
            for (int i = 0; i < 4; i++) {
                corner = getBlocIncludingNeighbors(x + AO_deltas[i][0], y + AO_deltas[i][1], z + AO_deltas[i][2]);
                left =   getBlocIncludingNeighbors(x + AO_deltas[i][0], y                  , z + AO_deltas[i][2]);
                right =  getBlocIncludingNeighbors(x + AO_deltas[i][0], y + AO_deltas[i][1], z                  );
        
                ao.push_back(3.0f - (BlocDatabase::getInstance().solidValue(corner) + BlocDatabase::getInstance().solidValue(left) + BlocDatabase::getInstance().solidValue(right)));
            }
            break;
        case FACE_WEST:
            AO_deltas = AO_West_deltas;
            for (int i = 0; i < 4; i++) {
                corner = getBlocIncludingNeighbors(x + AO_deltas[i][0], y + AO_deltas[i][1], z + AO_deltas[i][2]);
                left =   getBlocIncludingNeighbors(x + AO_deltas[i][0], y                  , z + AO_deltas[i][2]);
                right =  getBlocIncludingNeighbors(x + AO_deltas[i][0], y + AO_deltas[i][1], z                  );
        
                ao.push_back(3.0f - (BlocDatabase::getInstance().solidValue(corner) + BlocDatabase::getInstance().solidValue(left) + BlocDatabase::getInstance().solidValue(right)));
            }
            break;
        case FACE_BOTTOM:
            AO_deltas = AO_Bottom_deltas;
            for (int i = 0; i < 4; i++) {
                corner = getBlocIncludingNeighbors(x + AO_deltas[i][0], y + AO_deltas[i][1], z + AO_deltas[i][2]);
                left =   getBlocIncludingNeighbors(x                  , y + AO_deltas[i][1], z + AO_deltas[i][2]);
                right =  getBlocIncludingNeighbors(x + AO_deltas[i][0], y + AO_deltas[i][1], z                  );
        
                ao.push_back(3.0f - (BlocDatabase::getInstance().solidValue(corner) + BlocDatabase::getInstance().solidValue(left) + BlocDatabase::getInstance().solidValue(right)));
            }
            break;
        case FACE_TOP:
            AO_deltas = AO_Top_deltas;
            for (int i = 0; i < 4; i++) {
                corner = getBlocIncludingNeighbors(x + AO_deltas[i][0], y + AO_deltas[i][1], z + AO_deltas[i][2]);
                left =   getBlocIncludingNeighbors(x                  , y + AO_deltas[i][1], z + AO_deltas[i][2]);
                right =  getBlocIncludingNeighbors(x + AO_deltas[i][0], y + AO_deltas[i][1], z                  );
        
                ao.push_back(3.0f - (BlocDatabase::getInstance().solidValue(corner) + BlocDatabase::getInstance().solidValue(left) + BlocDatabase::getInstance().solidValue(right)));
            }
            break;
        case FACE_SOUTH:
            AO_deltas = AO_South_deltas;
            for (int i = 0; i < 4; i++) {
                corner = getBlocIncludingNeighbors(x + AO_deltas[i][0], y + AO_deltas[i][1], z + AO_deltas[i][2]);
                left =   getBlocIncludingNeighbors(x                  , y + AO_deltas[i][1], z + AO_deltas[i][2]);
                right =  getBlocIncludingNeighbors(x + AO_deltas[i][0], y                  , z + AO_deltas[i][2]);
        
                ao.push_back(3.0f - (BlocDatabase::getInstance().solidValue(corner) + BlocDatabase::getInstance().solidValue(left) + BlocDatabase::getInstance().solidValue(right)));
            }
            break;
        case FACE_NORTH:
            AO_deltas = AO_North_deltas;
            for (int i = 0; i < 4; i++) {
                corner = getBlocIncludingNeighbors(x + AO_deltas[i][0], y + AO_deltas[i][1], z + AO_deltas[i][2]);
                left =   getBlocIncludingNeighbors(x                  , y + AO_deltas[i][1], z + AO_deltas[i][2]);
                right =  getBlocIncludingNeighbors(x + AO_deltas[i][0], y                  , z + AO_deltas[i][2]);
        
                ao.push_back(3.0f - (BlocDatabase::getInstance().solidValue(corner) + BlocDatabase::getInstance().solidValue(left) + BlocDatabase::getInstance().solidValue(right)));
            }
            break;
        default:
            return;
        }
    }

void VoxelChunk::generateMesh() {
    //std::cout << "Generating mesh at chunk coords (" << m_chunkCoords.x << ", " << m_chunkCoords.y << ", " << m_chunkCoords.z << ")" << std::endl;
    m_opaqueMesh.vertices.clear();
    m_opaqueMesh.triangles.clear();
    m_opaqueMesh.uvs.clear();
    m_opaqueMesh.normals.clear();
    m_opaqueMesh.lights.clear();
    m_opaqueMesh.ao.clear();

    int neighbor;

    // Loop through all the cubes in the chunk. If a cube has a face that is not adjacent to another non opaque cube, add a face to the mesh
    for (int x = 0; x < m_sizeX; x++) {
        for (int y = 0; y < m_sizeY; y++) {
            for (int z = 0; z < m_sizeZ; z++) {
                if (m_cubes[x][y][z] != AIR && BlocDatabase::getInstance().isOpaque(m_cubes[x][y][z])) {
                    // Check all the adjacent cubes to see if they are air or leaves
                    neighbor = getBlocIncludingNeighbors(x - 1, y, z);
                    if (opaqueNeighborCheck(neighbor)) {
                        addSquareGeometry(m_opaqueMesh, m_cubes[x][y][z], FACE_EAST, x, y, z);
                        m_opaqueMesh.lights.push_back(getFaceLight(x, y, z, FACE_EAST));
                        m_opaqueMesh.lights.push_back(getFaceLight(x, y, z, FACE_EAST));
                        m_opaqueMesh.lights.push_back(getFaceLight(x, y, z, FACE_EAST));
                        m_opaqueMesh.lights.push_back(getFaceLight(x, y, z, FACE_EAST));
                        addAOValues(x, y, z, FACE_EAST, m_opaqueMesh.ao);
                    }
                    neighbor = getBlocIncludingNeighbors(x + 1, y, z);
                    if (opaqueNeighborCheck(neighbor)) {
                        addSquareGeometry(m_opaqueMesh, m_cubes[x][y][z], FACE_WEST, x, y, z);
                        m_opaqueMesh.lights.push_back(getFaceLight(x, y, z, FACE_WEST));
                        m_opaqueMesh.lights.push_back(getFaceLight(x, y, z, FACE_WEST));
                        m_opaqueMesh.lights.push_back(getFaceLight(x, y, z, FACE_WEST));
                        m_opaqueMesh.lights.push_back(getFaceLight(x, y, z, FACE_WEST));
                        addAOValues(x, y, z, FACE_WEST, m_opaqueMesh.ao);
                    }
                    neighbor = getBlocIncludingNeighbors(x, y - 1, z);
                    if (opaqueNeighborCheck(neighbor)) {
                        addSquareGeometry(m_opaqueMesh, m_cubes[x][y][z], FACE_BOTTOM, x, y, z);
                        m_opaqueMesh.lights.push_back(getFaceLight(x, y, z, FACE_BOTTOM));
                        m_opaqueMesh.lights.push_back(getFaceLight(x, y, z, FACE_BOTTOM));
                        m_opaqueMesh.lights.push_back(getFaceLight(x, y, z, FACE_BOTTOM));
                        m_opaqueMesh.lights.push_back(getFaceLight(x, y, z, FACE_BOTTOM));
                        addAOValues(x, y, z, FACE_BOTTOM, m_opaqueMesh.ao);
                    }
                    neighbor = getBlocIncludingNeighbors(x, y + 1, z);
                    if (opaqueNeighborCheck(neighbor)) {
                        addSquareGeometry(m_opaqueMesh, m_cubes[x][y][z], FACE_TOP, x, y, z);
                        m_opaqueMesh.lights.push_back(getFaceLight(x, y, z, FACE_TOP));
                        m_opaqueMesh.lights.push_back(getFaceLight(x, y, z, FACE_TOP));
                        m_opaqueMesh.lights.push_back(getFaceLight(x, y, z, FACE_TOP));
                        m_opaqueMesh.lights.push_back(getFaceLight(x, y, z, FACE_TOP));
                        addAOValues(x, y, z, FACE_TOP, m_opaqueMesh.ao);
                    }
                    neighbor = getBlocIncludingNeighbors(x, y, z - 1);
                    if (opaqueNeighborCheck(neighbor)) {
                        addSquareGeometry(m_opaqueMesh, m_cubes[x][y][z], FACE_SOUTH, x, y, z);
                        m_opaqueMesh.lights.push_back(getFaceLight(x, y, z, FACE_SOUTH));
                        m_opaqueMesh.lights.push_back(getFaceLight(x, y, z, FACE_SOUTH));
                        m_opaqueMesh.lights.push_back(getFaceLight(x, y, z, FACE_SOUTH));
                        m_opaqueMesh.lights.push_back(getFaceLight(x, y, z, FACE_SOUTH));
                        addAOValues(x, y, z, FACE_SOUTH, m_opaqueMesh.ao);
                    }
                    neighbor = getBlocIncludingNeighbors(x, y, z + 1);
                    if (opaqueNeighborCheck(neighbor)) {
                        addSquareGeometry(m_opaqueMesh, m_cubes[x][y][z], FACE_NORTH, x, y, z);
                        m_opaqueMesh.lights.push_back(getFaceLight(x, y, z, FACE_NORTH));
                        m_opaqueMesh.lights.push_back(getFaceLight(x, y, z, FACE_NORTH));
                        m_opaqueMesh.lights.push_back(getFaceLight(x, y, z, FACE_NORTH));
                        m_opaqueMesh.lights.push_back(getFaceLight(x, y, z, FACE_NORTH));
                        addAOValues(x, y, z, FACE_NORTH, m_opaqueMesh.ao);
                    }
                }
            }
        }
    }
    m_opaqueMesh.initializeBuffers();

    m_transparentMesh.vertices.clear();
    m_transparentMesh.triangles.clear();
    m_transparentMesh.uvs.clear();
    m_transparentMesh.normals.clear();
    m_transparentMesh.lights.clear();
    m_transparentMesh.ao.clear();

    // Loop through all the cubes in the chunk. If a cube has a face that is not adjacent to another non opaque cube, add a face to the mesh
    for (int x = 0; x < m_sizeX; x++) {
        for (int y = 0; y < m_sizeY; y++) {
            for (int z = 0; z < m_sizeZ; z++) {
                if (m_cubes[x][y][z] != AIR && !BlocDatabase::getInstance().isOpaque(m_cubes[x][y][z])) {
                    // Check all the adjacent cubes to see if they are air or leaves
                    neighbor = getBlocIncludingNeighbors(x - 1, y, z);
                    if (transparentNeighborCheck(neighbor, m_cubes[x][y][z], FACE_EAST)) {
                        addSquareGeometry(m_transparentMesh, m_cubes[x][y][z], FACE_EAST, x, y, z);
                        m_transparentMesh.lights.push_back(getFaceLight(x, y, z, FACE_EAST));
                        m_transparentMesh.lights.push_back(getFaceLight(x, y, z, FACE_EAST));
                        m_transparentMesh.lights.push_back(getFaceLight(x, y, z, FACE_EAST));
                        m_transparentMesh.lights.push_back(getFaceLight(x, y, z, FACE_EAST));
                        addAOValues(x, y, z, FACE_EAST, m_transparentMesh.ao);
                    }
                    neighbor = getBlocIncludingNeighbors(x + 1, y, z);
                    if (transparentNeighborCheck(neighbor, m_cubes[x][y][z], FACE_WEST)) {
                        addSquareGeometry(m_transparentMesh, m_cubes[x][y][z], FACE_WEST, x, y, z);
                        m_transparentMesh.lights.push_back(getFaceLight(x, y, z, FACE_WEST));
                        m_transparentMesh.lights.push_back(getFaceLight(x, y, z, FACE_WEST));
                        m_transparentMesh.lights.push_back(getFaceLight(x, y, z, FACE_WEST));
                        m_transparentMesh.lights.push_back(getFaceLight(x, y, z, FACE_WEST));
                        addAOValues(x, y, z, FACE_WEST, m_transparentMesh.ao);
                    }
                    neighbor = getBlocIncludingNeighbors(x, y - 1, z);
                    if (transparentNeighborCheck(neighbor, m_cubes[x][y][z], FACE_BOTTOM)) {
                        addSquareGeometry(m_transparentMesh, m_cubes[x][y][z], FACE_BOTTOM, x, y, z);
                        m_transparentMesh.lights.push_back(getFaceLight(x, y, z, FACE_BOTTOM));
                        m_transparentMesh.lights.push_back(getFaceLight(x, y, z, FACE_BOTTOM));
                        m_transparentMesh.lights.push_back(getFaceLight(x, y, z, FACE_BOTTOM));
                        m_transparentMesh.lights.push_back(getFaceLight(x, y, z, FACE_BOTTOM));
                        addAOValues(x, y, z, FACE_BOTTOM, m_transparentMesh.ao);
                    }
                    neighbor = getBlocIncludingNeighbors(x, y + 1, z);
                    if (transparentNeighborCheck(neighbor, m_cubes[x][y][z], FACE_TOP)) {
                        addSquareGeometry(m_transparentMesh, m_cubes[x][y][z], FACE_TOP, x, y, z, m_cubes[x][y][z] == WATER && getBlocIncludingNeighbors(x, y + 1, z) != WATER);
                        m_transparentMesh.lights.push_back(getFaceLight(x, y, z, FACE_TOP));
                        m_transparentMesh.lights.push_back(getFaceLight(x, y, z, FACE_TOP));
                        m_transparentMesh.lights.push_back(getFaceLight(x, y, z, FACE_TOP));
                        m_transparentMesh.lights.push_back(getFaceLight(x, y, z, FACE_TOP));
                        addAOValues(x, y, z, FACE_TOP, m_transparentMesh.ao);
                    }
                    neighbor = getBlocIncludingNeighbors(x, y, z - 1);
                    if (transparentNeighborCheck(neighbor, m_cubes[x][y][z], FACE_SOUTH)) {
                        addSquareGeometry(m_transparentMesh, m_cubes[x][y][z], FACE_SOUTH, x, y, z);
                        m_transparentMesh.lights.push_back(getFaceLight(x, y, z, FACE_SOUTH));
                        m_transparentMesh.lights.push_back(getFaceLight(x, y, z, FACE_SOUTH));
                        m_transparentMesh.lights.push_back(getFaceLight(x, y, z, FACE_SOUTH));
                        m_transparentMesh.lights.push_back(getFaceLight(x, y, z, FACE_SOUTH));
                        addAOValues(x, y, z, FACE_SOUTH, m_transparentMesh.ao);
                    }
                    neighbor = getBlocIncludingNeighbors(x, y, z + 1);
                    if (transparentNeighborCheck(neighbor, m_cubes[x][y][z], FACE_NORTH)) {
                        addSquareGeometry(m_transparentMesh, m_cubes[x][y][z], FACE_NORTH, x, y, z);
                        m_transparentMesh.lights.push_back(getFaceLight(x, y, z, FACE_NORTH));
                        m_transparentMesh.lights.push_back(getFaceLight(x, y, z, FACE_NORTH));
                        m_transparentMesh.lights.push_back(getFaceLight(x, y, z, FACE_NORTH));
                        m_transparentMesh.lights.push_back(getFaceLight(x, y, z, FACE_NORTH));
                        addAOValues(x, y, z, FACE_NORTH, m_transparentMesh.ao);
                    }
                }
            }
        }
    }
    m_transparentMesh.initializeBuffers();
    dirty = false;
}

void VoxelChunk::draw(GLuint programID) {
    if (dirty) generateMesh();
    GLuint modelMatrixId = glGetUniformLocation(programID, "ModelMatrix");
    glUniformMatrix4fv(modelMatrixId, 1, false, &ModelMatrix[0][0]);

    //TextureAtlas::getInstance().bind(programID);
    PBRTextureAtlas::getInstance().bind(programID);
    m_opaqueMesh.draw(programID);
}

void VoxelChunk::drawTransparent(GLuint programID) {
    GLuint modelMatrixId = glGetUniformLocation(programID, "ModelMatrix");
    glUniformMatrix4fv(modelMatrixId, 1, false, &ModelMatrix[0][0]);
    
    //TextureAtlas::getInstance().bind(programID);
    PBRTextureAtlas::getInstance().bind(programID);
    m_transparentMesh.draw(programID);
}   

void VoxelChunk::cleanupBuffers() {
    m_opaqueMesh.cleanupBuffers();
}

bool VoxelChunk::contains(glm::vec3 point) {
    return point.x >= getWorldPosition().x && point.x <= getWorldPosition().x + m_sizeX &&
           point.y >= getWorldPosition().y && point.y <= getWorldPosition().y + m_sizeY &&
           point.z >= getWorldPosition().z && point.z <= getWorldPosition().z + m_sizeZ;
}

bool VoxelChunk::contains(Ray ray) {
    return contains(ray.origin) || contains(ray.origin + ray.direction);
}

bool VoxelChunk::intersects(Ray ray, float maxDistance) {
    return contains(ray) || ray.rayIntersectsAABB(ray, getWorldPosition(), getWorldPosition() + glm::vec3(m_sizeX, m_sizeY, m_sizeZ), maxDistance);
}

// Move Constructor
VoxelChunk::VoxelChunk(VoxelChunk&& other) noexcept
    : SceneNode(std::move(other)), m_sizeX(other.m_sizeX), m_sizeY(other.m_sizeY), m_sizeZ(other.m_sizeZ), m_cubes(std::move(other.m_cubes)), m_lights(std::move(other.m_lights)) {
}

// Move Assignment Operator
VoxelChunk& VoxelChunk::operator=(VoxelChunk&& other) noexcept {
    if (this != &other) {
        cleanup(); // Free existing resources
        SceneNode::operator=(std::move(other));
        m_sizeX = other.m_sizeX;
        m_sizeY = other.m_sizeY;
        m_sizeZ = other.m_sizeZ;
        m_cubes = std::move(other.m_cubes);
        m_lights = std::move(other.m_lights);
    }
    return *this;
}

void VoxelChunk::allocateCubes() {
    m_cubes = std::vector<std::vector<std::vector<int>>>(m_sizeX, std::vector<std::vector<int>>(m_sizeY, std::vector<int>(m_sizeZ, AIR)));
    m_lights = std::vector<std::vector<std::vector<int>>>(m_sizeX, std::vector<std::vector<int>>(m_sizeY, std::vector<int>(m_sizeZ, MIN_LIGHT)));
}

void VoxelChunk::cleanup() {
    m_cubes.clear();
    cleanupBuffers();
}

void VoxelChunk::markDirtyNeighbors(int x, int y, int z) {
    // Mark neighboring chunks as dirty if the block is on the edge of the chunk
    if (x == 0 || x == m_sizeX - 1 || y == 0 || y == m_sizeY - 1 || z == 0 || z == m_sizeZ - 1) {
        std::vector<std::shared_ptr<VoxelChunk>> neighbors;
        if (x == 0) {
            neighbors.push_back(m_world->getChunk(m_chunkCoords.x - 1, m_chunkCoords.y, m_chunkCoords.z));
        } else if (x == m_sizeX - 1) {
            neighbors.push_back(m_world->getChunk(m_chunkCoords.x + 1, m_chunkCoords.y, m_chunkCoords.z));
        }
        if (y == 0) {
            neighbors.push_back(m_world->getChunk(m_chunkCoords.x, m_chunkCoords.y - 1, m_chunkCoords.z));
        } else if (y == m_sizeY - 1) {
            neighbors.push_back(m_world->getChunk(m_chunkCoords.x, m_chunkCoords.y + 1, m_chunkCoords.z));
        }
        if (z == 0) {
            neighbors.push_back(m_world->getChunk(m_chunkCoords.x, m_chunkCoords.y, m_chunkCoords.z - 1));
        } else if (z == m_sizeZ - 1) {
            neighbors.push_back(m_world->getChunk(m_chunkCoords.x, m_chunkCoords.y, m_chunkCoords.z + 1));
        }
        for (auto &neighbor: neighbors) {
            if (neighbor) {
                neighbor->dirty = true;
            }
        }
    }
}
