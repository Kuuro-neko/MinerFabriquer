#include "EnderPearl.hpp"
#include <utils/GLUtils.hpp>
#include "TP/Scene/BlocTypes.hpp"
#include <TP/Scene/World.hpp>

EnderPearl::EnderPearl(Character *character, glm::vec3 velocity, float radius, World *world, GLuint programID)
    : Projectile(character->getWorldPosition(), velocity * ENDERPEARL_PROJ_SPEED, radius,
                 ENDERPEARL_PROJ_SPEED, world, programID)
{
    m_character = character;
    generateMesh();
}

EnderPearl::~EnderPearl()
{
    // Base class clear() will be called
}

void EnderPearl::generateMesh()
{
    // TODO: Change to Ender Pearl block when available
    addBlockToMesh(MAGMA);
}

void EnderPearl::onCollision(const glm::vec3 &collisionNormal)
{
    // Handle collision with the world
    glm::vec3 collisionPoint = getWorldPosition() + collisionNormal * m_radius;
    int x = static_cast<int>(collisionPoint.x);
    int y = static_cast<int>(collisionPoint.y);
    int z = static_cast<int>(collisionPoint.z);

    teleport(x, y, z);

    // Call base expiration to remove from world
    onExpire();
}

void EnderPearl::draw(GLuint programID)
{
    TextureManager::getInstance().getPBRTexture("blocks")->bind(programID);
    GLuint modelMatrixId = glGetUniformLocation(programID, "ModelMatrix");
    glUniformMatrix4fv(modelMatrixId, 1, false, &ModelMatrix[0][0]);
    m_mesh->draw(programID);
}

void EnderPearl::teleport(int x, int y, int z)
{
    // Teleport the player to the specified position
    m_character->setWorldPosition(x, y + 1, z); // +1 to prevent teleporting inside a block
}