#include "Projectile.hpp"

Projectile::Projectile(glm::vec3 position, glm::vec3 velocity, float radius, float speed, World *world, GLuint programID)
    : Entity(), m_world(world), m_velocity(velocity), m_radius(radius), m_speed(speed), m_programID(programID)
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
    if (m_time > m_timeToLive)
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

bool Projectile::checkCollisions(float deltaTime, glm::vec3 &outCollisionNormal)
{
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
    // Default behavior - to be overridden by derived classes
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