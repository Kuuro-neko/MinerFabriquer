#pragma once

#include <glm/glm.hpp>
#include <TP/Scene/SceneNode.hpp>
#include <TP/Scene/World.hpp>
#include <memory>



class Projectile : public Entity
{
protected:
    World *m_world;
    glm::vec3 m_velocity;
    float m_radius;
    float m_speed;
    GLuint m_programID;


    // Bounding box
    glm::vec3 m_AABBmin;
    glm::vec3 m_AABBmax;

    // Lifetime management
    float m_time = 0.0f;
    float m_timeToLive = 5.0f;

    // Common mesh that all projectiles use
    std::shared_ptr<VoxelMeshObject> m_mesh;

public:
    Projectile(glm::vec3 position, glm::vec3 velocity, float radius, float speed,
               World *world, GLuint programID);
    virtual ~Projectile();

    // Getters and setters
    inline void setVelocity(glm::vec3 velocity) { m_velocity = velocity; }
    inline glm::vec3 getVelocity() const { return m_velocity; }
    inline void setRadius(float radius) { m_radius = radius; }
    inline float getRadius() const { return m_radius; }
    inline void setTimeToLive(float ttl) { m_timeToLive = ttl; }

    // Physics and update methods
    virtual void update(float deltaTime);
    virtual void onCollision(const glm::vec3 &collisionNormal);

    // Event handlers
    virtual void onExpire();

    // Mesh generation
    virtual void generateMesh() = 0;

    // Bounding box methods
    virtual void updateBoundingBox() override;
    virtual glm::vec3 getMinBoundingBox() override;
    virtual glm::vec3 getMaxBoundingBox() override;

    // Common cleanup method with default implementation
    virtual void clear();

protected:
    // Helper methods
    bool checkCollisions(float deltaTime, glm::vec3 &outCollisionNormal);
    bool isExpired() const { return m_time >= m_timeToLive; }
    void addBlockToMesh(int blockType);
};