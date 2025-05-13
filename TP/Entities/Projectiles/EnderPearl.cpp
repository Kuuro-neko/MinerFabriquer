#include "EnderPearl.hpp"
#include <utils/GLUtils.hpp>
#include "TP/Scene/BlocTypes.hpp"
#include <TP/Scene/World.hpp>

EnderPearl::EnderPearl(Character *character, glm::vec3 velocity, float radius, World *world, GLuint programID)
    : Projectile(character->getWorldPosition(), velocity * ENDERPEARL_PROJ_SPEED, radius, ENDERPEARL_PROJ_SPEED, world, programID)
{
    m_character = character;
    generateMesh();
}

EnderPearl::~EnderPearl()
{
    clear();
}

void EnderPearl::generateMesh()
{
    BlockData *tntData = BlocDatabase::getInstance().getBloc(TNT); // TODO : Change to Ender Pearl
    m_enderPearlMesh = std::make_shared<VoxelMeshObject>();

    m_enderPearlMesh->vertices.clear();
    m_enderPearlMesh->triangles.clear();
    m_enderPearlMesh->uvs.clear();
    m_enderPearlMesh->normals.clear();
    m_enderPearlMesh->lights.clear();
    m_enderPearlMesh->ao.clear();

    addSquareGeometry(m_enderPearlMesh, TNT, FACE_BOTTOM, 0.f, 0.f, 0.f, false, m_radius);
    addSquareGeometry(m_enderPearlMesh, TNT, FACE_TOP, 0.f, 0.f, 0.f, false, m_radius);
    addSquareGeometry(m_enderPearlMesh, TNT, FACE_WEST, 0.f, 0.f, 0.f, false, m_radius);
    addSquareGeometry(m_enderPearlMesh, TNT, FACE_EAST, 0.f, 0.f, 0.f, false, m_radius);
    addSquareGeometry(m_enderPearlMesh, TNT, FACE_NORTH, 0.f, 0.f, 0.f, false, m_radius);
    addSquareGeometry(m_enderPearlMesh, TNT, FACE_SOUTH, 0.f, 0.f, 0.f, false, m_radius);

    for (int i = 0; i < m_enderPearlMesh->vertices.size(); i++)
    {
        m_enderPearlMesh->lights.push_back(15);
        m_enderPearlMesh->ao.push_back(3);
    }

    m_enderPearlMesh->initializeBuffers();
}

void EnderPearl::draw(GLuint programID)
{
    PBRTextureAtlas::getInstance().bind(programID);
    GLuint modelMatrixId = glGetUniformLocation(programID, "ModelMatrix");
    glUniformMatrix4fv(modelMatrixId, 1, false, &ModelMatrix[0][0]);
    m_enderPearlMesh->draw(programID);
}

void EnderPearl::onExpire()
{

    m_world->removeTNT(this); // TODO CHANGE NAME
}

void EnderPearl::clear()
{
    if (m_enderPearlMesh)
    {
        m_enderPearlMesh->cleanupBuffers();
        m_enderPearlMesh->vertices.clear();
        m_enderPearlMesh->triangles.clear();
        m_enderPearlMesh->uvs.clear();
        m_enderPearlMesh->normals.clear();
        m_enderPearlMesh->lights.clear();
        m_enderPearlMesh->ao.clear();
    }
}

void EnderPearl::onCollision(const glm::vec3 &collisionNormal)
{
    // Handle collision with the world
    glm::vec3 collisionPoint = getWorldPosition() + collisionNormal * m_radius;
    int x = static_cast<int>(collisionPoint.x);
    int y = static_cast<int>(collisionPoint.y);
    int z = static_cast<int>(collisionPoint.z);

    teleport(x, y, z);
    onExpire(); // Call onExpire to remove the projectile
}

void EnderPearl::teleport(int x, int y, int z)
{

    // Teleport the player to the specified position
    m_character->setWorldPosition(x, y+1, z); // =1 to prevent teleporting inside a block
}
