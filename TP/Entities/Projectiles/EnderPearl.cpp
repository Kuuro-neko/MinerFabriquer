#include "EnderPearl.hpp"
#include <utils/GLUtils.hpp>
#include "../../Database/BlocTypes.hpp"
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
    addBillBoardToMesh(ENDER_PEARL);
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
    glDisable(GL_CULL_FACE);
    // depth test thing
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // orienter vers la caméra
    glm::vec3 cameraPos = m_world->getCamera()->getPosition();
    glm::vec3 objectPos = getWorldPosition();
    
    glm::vec3 up = VEC_UP;
    
    glm::vec3 look = glm::normalize(cameraPos - objectPos);
    glm::vec3 right = glm::normalize(glm::cross(up, look));
    up = glm::cross(look, right);
    
    glm::mat4 ModelMatrix = glm::mat4(1.0f);
    
    // set la rotation
    ModelMatrix[0] = glm::vec4(right, 0.0f);
    ModelMatrix[1] = glm::vec4(up, 0.0f);
    ModelMatrix[2] = glm::vec4(look, 0.0f);
    
    // set translation et scaling
    ModelMatrix[3] = glm::vec4(objectPos, 1.0f);
    ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.5f, 0.5f, 0.5f));


    TextureManager::getInstance().getPBRTexture("items")->bind(programID);
    GLuint modelMatrixId = glGetUniformLocation(programID, "ModelMatrix");
    glUniformMatrix4fv(modelMatrixId, 1, false, &ModelMatrix[0][0]);
    m_mesh->draw(programID);

    glEnable(GL_CULL_FACE);
}

void EnderPearl::teleport(int x, int y, int z)
{
    // Teleport the player to the specified position
    m_character->setWorldPosition(x, y + 1, z); // +1 to prevent teleporting inside a block
}