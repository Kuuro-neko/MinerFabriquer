#include <TP/Scene/VoxelChunk.hpp>
#include <TP/Scene/World.hpp>

VoxelChunk::VoxelChunk(int sizeX, int sizeY, int sizeZ) : SceneNode(Transform(), new MeshObject(), nullptr), m_sizeX(sizeX), m_sizeY(sizeY), m_sizeZ(sizeZ) {
    allocateCubes();
}
VoxelChunk::VoxelChunk() : VoxelChunk(DEFAULT_CHUNK_SIZE, DEFAULT_CHUNK_SIZE, DEFAULT_CHUNK_HEIGHT) {
    allocateCubes();
}
VoxelChunk::~VoxelChunk() {
    cleanup();
}

bool VoxelChunk::setBloc(int x, int y, int z, int bloc, bool genMesh) {
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
    if(genMesh) generateMesh();
    return true;
}

bool VoxelChunk::generationSetBloc(int x, int y, int z, int bloc) {
    if (x < 0 || x >= m_sizeX || y < 0 || y >= m_sizeY || z < 0 || z >= m_sizeZ) {
        //std::cout << "Error: Out of bounds" << std::endl;
        return false;
    }
    m_cubes[x][y][z] = bloc;
    return true;
}

int VoxelChunk::getBloc(int x, int y, int z) {
    if (x < 0 || x >= m_sizeX || y < 0 || y >= m_sizeY || z < 0 || z >= m_sizeZ) {
        // POUR MATHIS : QUAND ON SORT DU CHUNK CETTE FONCTION CASSE
        // On devrait peut etre ajouter des exceptions ? xD et enlever mes vieux return -1, -2
        return -1;
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

int VoxelChunk::playerRemoveBlock(int x, int y, int z) {
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

int VoxelChunk::removeBlock(int x, int y, int z) {
    int id = m_cubes[x][y][z];
    m_cubes[x][y][z] = AIR;
    generateMesh();
    return id;
}

bool neighborCheck(int neighbor) {
    return neighbor == AIR || neighbor == LEAVES_OAK; // || neighbor == OUT_OF_BOUNDS_BLOC; // to display chunk sides even if it's out of bounds
}

void VoxelChunk::generateMesh() {
    m_mesh->vertices.clear();
    m_mesh->triangles.clear();
    m_mesh->uvs.clear();

    int neighbor;

    // Loop through all the cubes in the chunk. If a cube has a face that is not adjacent to another non opaque cube, add a face to the mesh
    for (int x = 0; x < m_sizeX; x++) {
        for (int y = 0; y < m_sizeY; y++) {
            for (int z = 0; z < m_sizeZ; z++) {
                if (m_cubes[x][y][z] != AIR) {
                    // Check all the adjacent cubes to see if they are air or leaves
                    neighbor = getBlocIncludingNeighbors(x - 1, y, z);
                    if (neighborCheck(neighbor)) {
                        addSquareGeometry(m_mesh->vertices, m_mesh->triangles, m_mesh->uvs, m_cubes[x][y][z], BLOC_LEFT, x, y, z);
                    }
                    neighbor = getBlocIncludingNeighbors(x + 1, y, z);
                    if (neighborCheck(neighbor)) {
                        addSquareGeometry(m_mesh->vertices, m_mesh->triangles, m_mesh->uvs, m_cubes[x][y][z], BLOC_RIGHT, x, y, z);
                    }
                    neighbor = getBlocIncludingNeighbors(x, y - 1, z);
                    if (neighborCheck(neighbor)) {
                        addSquareGeometry(m_mesh->vertices, m_mesh->triangles, m_mesh->uvs, m_cubes[x][y][z], BLOC_BOTTOM, x, y, z);
                    }
                    neighbor = getBlocIncludingNeighbors(x, y + 1, z);
                    if (neighborCheck(neighbor)) {
                        addSquareGeometry(m_mesh->vertices, m_mesh->triangles, m_mesh->uvs, m_cubes[x][y][z], BLOC_TOP, x, y, z);
                    }
                    neighbor = getBlocIncludingNeighbors(x, y, z - 1);
                    if (neighborCheck(neighbor)) {
                        addSquareGeometry(m_mesh->vertices, m_mesh->triangles, m_mesh->uvs, m_cubes[x][y][z], BLOC_FRONT, x, y, z);
                    }
                    neighbor = getBlocIncludingNeighbors(x, y, z + 1);
                    if (neighborCheck(neighbor)) {
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

void VoxelChunk::cleanupBuffers() {
    m_mesh->cleanupBuffers();
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
    : SceneNode(std::move(other)), m_sizeX(other.m_sizeX), m_sizeY(other.m_sizeY), m_sizeZ(other.m_sizeZ), m_cubes(other.m_cubes) {
    other.m_cubes = nullptr; // Nullify the source pointer
}

// Move Assignment Operator
VoxelChunk& VoxelChunk::operator=(VoxelChunk&& other) noexcept {
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
VoxelChunk::VoxelChunk(const VoxelChunk& other)
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
VoxelChunk& VoxelChunk::operator=(const VoxelChunk& other) {
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

void VoxelChunk::allocateCubes() {
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

void VoxelChunk::cleanup() {
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