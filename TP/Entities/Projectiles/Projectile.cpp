#include "Projectile.hpp"
#include <TP/Scene/BlocTypes.hpp>
#include <utils/GLUtils.hpp>

Projectile::Projectile(glm::vec3 position, glm::vec3 velocity, float radius,
                       float speed, World *world, GLuint programID)
    : Entity(), m_world(world), m_velocity(velocity), m_radius(radius),
      m_speed(speed), m_programID(programID)
{
    SceneNode::translate(position);
    updateBoundingBox();
}

Projectile::~Projectile()
{
    // Base class virtual destructor
}

void Projectile::update(float deltaTime)
{
    // Apply gravity and movement
    translate(m_velocity * deltaTime);
    m_velocity.y -= 9.81f * deltaTime; // Apply gravity

    // Check for expiration
    m_time += deltaTime;
    if (isExpired())
    {
        onExpire();
        return;
    }

    // Update bounding box
    updateBoundingBox();

    // Check for collisions
    glm::vec3 collisionNormal(0.0f);
    if (checkCollisions(deltaTime, collisionNormal))
    {
        onCollision(collisionNormal);
    }
}

void Projectile::draw(GLuint programID)
{
    if (m_mesh)
    {
        PBRTextureAtlas::getInstance().bind(programID);
        GLuint modelMatrixId = glGetUniformLocation(programID, "ModelMatrix");
        glUniformMatrix4fv(modelMatrixId, 1, false, &ModelMatrix[0][0]);
        m_mesh->draw(programID);
    }
}

bool Projectile::checkCollisions(float deltaTime, glm::vec3 &outCollisionNormal)
{
    // Existing collision detection code
    glm::vec3 minBB = getMinBoundingBox();
    glm::vec3 maxBB = getMaxBoundingBox();

    bool hasCollided = false;
    outCollisionNormal = glm::vec3(0.0f);

    for (int x = static_cast<int>(std::floor(minBB.x)); x <= static_cast<int>(std::floor(maxBB.x)); ++x)
    {
        for (int y = static_cast<int>(std::floor(minBB.y)); y <= static_cast<int>(std::floor(maxBB.y)); ++y)
        {
            for (int z = static_cast<int>(std::floor(minBB.z)); z <= static_cast<int>(std::floor(maxBB.z)); ++z)
            {
                int blockId = m_world->getBloc(x, y, z);

                if (blockId != AIR && blockId != WATER)
                {
                    // Existing collision response calculation
                    glm::vec3 blockPosition = glm::vec3(x, y, z);
                    glm::vec3 blockMin = blockPosition;
                    glm::vec3 blockMax = blockPosition + glm::vec3(1.0f);

                    // Check if the bounding box intersects with the block
                    if (maxBB.x > blockMin.x && minBB.x < blockMax.x &&
                        maxBB.y > blockMin.y && minBB.y < blockMax.y &&
                        maxBB.z > blockMin.z && minBB.z < blockMax.z)
                    {
                        // Calculate the overlaps on each axis
                        float overlapX = std::min(maxBB.x - blockMin.x, blockMax.x - minBB.x);
                        float overlapY = std::min(maxBB.y - blockMin.y, blockMax.y - minBB.y);
                        float overlapZ = std::min(maxBB.z - blockMin.z, blockMax.z - minBB.z);

                        hasCollided = true;

                        if (overlapX < overlapY && overlapX < overlapZ)
                        { // X axis collision
                            float centerX = getWorldPosition().x + m_radius / 2;
                            if (centerX < blockPosition.x)
                            {
                                outCollisionNormal += glm::vec3(-1, 0, 0); // Left collision
                            }
                            else
                            {
                                outCollisionNormal += glm::vec3(1, 0, 0); // Right collision
                            }
                        }
                        else if (overlapY < overlapX && overlapY < overlapZ)
                        { // Y axis collision
                            float centerY = getWorldPosition().y + m_radius / 2;
                            if (centerY < blockPosition.y)
                            {
                                outCollisionNormal += glm::vec3(0, -1, 0); // Bottom collision
                            }
                            else
                            {
                                outCollisionNormal += glm::vec3(0, 1, 0); // Top collision
                            }
                        }
                        else
                        { // Z axis collision
                            float centerZ = getWorldPosition().z + m_radius / 2;
                            if (centerZ < blockPosition.z)
                            {
                                outCollisionNormal += glm::vec3(0, 0, -1); // Back collision
                            }
                            else
                            {
                                outCollisionNormal += glm::vec3(0, 0, 1); // Front collision
                            }
                        }
                    }
                }
            }
        }
    }

    if (hasCollided && glm::length(outCollisionNormal) > 0)
    {
        outCollisionNormal = glm::normalize(outCollisionNormal);
        return true;
    }

    return false;
}

void Projectile::onCollision(const glm::vec3 &collisionNormal)
{
    // Default collision behavior - bounce
    float bounceFactor = 0.4f;
    float dotProduct = glm::dot(m_velocity, collisionNormal);
    m_velocity = m_velocity - (1.0f + bounceFactor) * dotProduct * collisionNormal;

    // Energy loss
    m_velocity *= 0.9f;
}

void Projectile::onExpire()
{
    // Base implementation - remove from world
    m_world->removeTNT(this);
}

void Projectile::updateBoundingBox()
{
    m_AABBmin = getWorldPosition();
    m_AABBmax = getWorldPosition() + glm::vec3(m_radius, m_radius, m_radius);
}

glm::vec3 Projectile::getMinBoundingBox()
{
    return m_AABBmin;
}

glm::vec3 Projectile::getMaxBoundingBox()
{
    return m_AABBmax;
}

void Projectile::clear()
{
    if (m_mesh)
    {
        m_mesh->cleanupBuffers();
        m_mesh->vertices.clear();
        m_mesh->triangles.clear();
        m_mesh->uvs.clear();
        m_mesh->normals.clear();
        m_mesh->lights.clear();
        m_mesh->ao.clear();
    }
}

void Projectile::addBlockToMesh(int blockType)
{
    if (!m_mesh)
    {
        m_mesh = std::make_shared<VoxelMeshObject>();
    }

    m_mesh->vertices.clear();
    m_mesh->triangles.clear();
    m_mesh->uvs.clear();
    m_mesh->normals.clear();
    m_mesh->lights.clear();
    m_mesh->ao.clear();

    // Add geometry for all faces
    addSquareGeometry(m_mesh, blockType, FACE_BOTTOM, 0.f, 0.f, 0.f, false, m_radius);
    addSquareGeometry(m_mesh, blockType, FACE_TOP, 0.f, 0.f, 0.f, false, m_radius);
    addSquareGeometry(m_mesh, blockType, FACE_WEST, 0.f, 0.f, 0.f, false, m_radius);
    addSquareGeometry(m_mesh, blockType, FACE_EAST, 0.f, 0.f, 0.f, false, m_radius);
    addSquareGeometry(m_mesh, blockType, FACE_NORTH, 0.f, 0.f, 0.f, false, m_radius);
    addSquareGeometry(m_mesh, blockType, FACE_SOUTH, 0.f, 0.f, 0.f, false, m_radius);

    // Add lighting information
    for (int i = 0; i < m_mesh->vertices.size(); i++)
    {
        m_mesh->lights.push_back(15);
        m_mesh->ao.push_back(3);
    }

    m_mesh->initializeBuffers();
}