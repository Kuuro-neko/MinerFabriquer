#include <TP/Entities/Projectiles/TNTProjectile.hpp>
#include <TP/Scene/BlocTypes.hpp>
#include <TP/Entities/HumanoidEntity.hpp>
#include <utils/GLUtils.hpp>
#include "TNTProjectile.hpp"


TNTProjectile::TNTProjectile(glm::vec3 position, glm::vec3 velocity, float radius, World *world, GLuint programID) : Entity()
{
    m_programID = programID;
    m_world = world;
    m_velocity = velocity * speed;
    m_radius = radius;
    generateTNTMesh();
    SceneNode::translate(position);
}

TNTProjectile::~TNTProjectile()
{
    clear();
    m_tntMesh->cleanupBuffers();
}

void TNTProjectile::explode(int x, int y, int z)
{;
    m_world->setBloc(x, y, z, AIR);

    // Create an explosion effect
    for (int i = -m_explosionRadius; i <= m_explosionRadius; ++i)
    {
        for (int j = -m_explosionRadius; j <= m_explosionRadius; ++j)
        {
            for (int k = -m_explosionRadius; k <= m_explosionRadius; ++k)
            {
                float dist = std::sqrt(i * i + j * j + k * k);
                if (dist > m_explosionRadius)
                    continue;
                m_world->removeBlock(x + i, y + j, z + k);
            }
        }
    }

    m_world->removeTNT(this);
}

void TNTProjectile::generateTNTMesh()
{
    BlockData* tntData = BlocDatabase::getInstance().getBloc(TNT);
    m_tntMesh = std::make_shared<VoxelMeshObject>();

    m_tntMesh->vertices.clear();
    m_tntMesh->triangles.clear();
    m_tntMesh->uvs.clear();
    m_tntMesh->normals.clear();
    m_tntMesh->lights.clear();
    m_tntMesh->ao.clear();

    addSquareGeometry(m_tntMesh, TNT, FACE_BOTTOM, 0.f, 0.f, 0.f, false, m_radius);
    addSquareGeometry(m_tntMesh, TNT, FACE_TOP, 0.f, 0.f, 0.f, false, m_radius);
    addSquareGeometry(m_tntMesh, TNT, FACE_WEST, 0.f, 0.f, 0.f, false, m_radius);
    addSquareGeometry(m_tntMesh, TNT, FACE_EAST, 0.f, 0.f, 0.f, false, m_radius);
    addSquareGeometry(m_tntMesh, TNT, FACE_NORTH, 0.f, 0.f, 0.f, false, m_radius);
    addSquareGeometry(m_tntMesh, TNT, FACE_SOUTH, 0.f, 0.f, 0.f, false, m_radius);

    for (int i = 0; i < m_tntMesh->vertices.size(); i++)
    {
        m_tntMesh->lights.push_back(15);
        m_tntMesh->ao.push_back(3);
    }

    m_tntMesh->initializeBuffers();
}

void TNTProjectile::draw(GLuint programID)
{
    PBRTextureAtlas::getInstance().bind(programID);
    GLuint modelMatrixId = glGetUniformLocation(programID, "ModelMatrix");
    glUniformMatrix4fv(modelMatrixId, 1, false, &ModelMatrix[0][0]);
    m_tntMesh->draw(programID);
}

void TNTProjectile::update(float deltaTime)
{
    translate(m_velocity * deltaTime);
    m_velocity.y -= 9.81f * deltaTime; // Apply gravity
    if (m_time > m_timeToLive)
    {
        explode(getWorldPosition().x, getWorldPosition().y, getWorldPosition().z);
    }
    m_time += deltaTime;
    updateBoundingBox();

    // Check for collisions with blocks
    glm::vec3 minBB = getMinBoundingBox();
    glm::vec3 maxBB = getMaxBoundingBox();
    
    // Store the original velocity for reflection calculations
    glm::vec3 originalVelocity = m_velocity;
    bool hasCollided = false;
    glm::vec3 collisionNormal(0.0f);

    for (int x = static_cast<int>(std::floor(minBB.x)); x <= static_cast<int>(std::floor(maxBB.x)); ++x)
    {
        for (int y = static_cast<int>(std::floor(minBB.y)); y <= static_cast<int>(std::floor(maxBB.y)); ++y)
        {
            for (int z = static_cast<int>(std::floor(minBB.z)); z <= static_cast<int>(std::floor(maxBB.z)); ++z)
            {
                int blockId = m_world->getBloc(x, y, z);

                if (blockId != AIR && blockId != WATER)
                {
                    glm::vec3 blockPosition = glm::vec3(x, y, z);
                    glm::vec3 blockMin = blockPosition;
                    glm::vec3 blockMax = blockPosition + glm::vec3(1.0f);
                    
                    // Check if the bounding box of the TNT intersects with the block
                    if (maxBB.x > blockMin.x && minBB.x < blockMax.x &&
                        maxBB.y > blockMin.y && minBB.y < blockMax.y &&
                        maxBB.z > blockMin.z && minBB.z < blockMax.z)
                    {
                        // Calculate the overlaps on each axis
                        float overlapX = std::min(maxBB.x - blockMin.x, blockMax.x - minBB.x);
                        float overlapY = std::min(maxBB.y - blockMin.y, blockMax.y - minBB.y);
                        float overlapZ = std::min(maxBB.z - blockMin.z, blockMax.z - minBB.z);

                        // Determine collision normal based on the smallest overlap
                        hasCollided = true;
                        
                        if (overlapX < overlapY && overlapX < overlapZ)
                        { // X axis collision
                            float centerX = getWorldPosition().x + m_radius/2;
                            if (centerX < blockPosition.x) {
                                collisionNormal += glm::vec3(-1, 0, 0); // Left collision
                            } else {
                                collisionNormal += glm::vec3(1, 0, 0);  // Right collision
                            }
                        }
                        else if (overlapY < overlapX && overlapY < overlapZ)
                        { // Y axis collision
                            float centerY = getWorldPosition().y + m_radius/2;
                            if (centerY < blockPosition.y) {
                                collisionNormal += glm::vec3(0, -1, 0); // Bottom collision
                            } else {
                                collisionNormal += glm::vec3(0, 1, 0);  // Top collision
                            }
                        }
                        else
                        { // Z axis collision
                            float centerZ = getWorldPosition().z + m_radius/2;
                            if (centerZ < blockPosition.z) {
                                collisionNormal += glm::vec3(0, 0, -1); // Back collision
                            } else {
                                collisionNormal += glm::vec3(0, 0, 1);  // Front collision
                            }
                        }
                    }
                }
            }
        }
    }

    // Apply bounce physics if there was a collision
    if (hasCollided && glm::length(collisionNormal) > 0) {
        // Normalize the collision normal
        collisionNormal = glm::normalize(collisionNormal);
        float bounceFactor = 0.4f;
        //  v' = v - 2 * dot(v, n) * n
        float dotProduct = glm::dot(originalVelocity, collisionNormal);
        m_velocity = originalVelocity - (1.0f + bounceFactor) * dotProduct * collisionNormal;
        
        m_velocity *= 0.9f; // energy loss
    }
}

void TNTProjectile::updateBoundingBox()
{
    AABBmin = getWorldPosition();
    AABBmax = getWorldPosition() + glm::vec3(m_radius, m_radius, m_radius);
}

glm::vec3 TNTProjectile::getMinBoundingBox()
{
    return AABBmin;
}

glm::vec3 TNTProjectile::getMaxBoundingBox()
{
    return AABBmax;
}

void TNTProjectile::clear()
{
    m_tntMesh->cleanupBuffers();
    m_tntMesh->vertices.clear();
    m_tntMesh->triangles.clear();
    m_tntMesh->uvs.clear();
    m_tntMesh->normals.clear();
    m_tntMesh->lights.clear();
    m_tntMesh->ao.clear();
}
