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

    glm::vec3 m_AABBmin;
    glm::vec3 m_AABBmax;

    float m_time = 0.0f;
    float m_timeToLive = 5.0f; // Default time to live

public:
    Projectile(glm::vec3 position, glm::vec3 velocity, float radius, float speed, World *world, GLuint programID);
    virtual ~Projectile();

    // Getters and setters
    inline void setVelocity(glm::vec3 velocity) { m_velocity = velocity; }
    inline glm::vec3 getVelocity() const { return m_velocity; }
    inline void setRadius(float radius) { m_radius = radius; }
    inline float getRadius() const { return m_radius; }
    inline void setTimeToLive(float ttl) { m_timeToLive = ttl; }

    // Virtual methods that can be overridden
    virtual void draw(GLuint programID) override = 0; // Pure virtual
    virtual void update(float deltaTime);
    virtual void onCollision(const glm::vec3 &collisionNormal);
    virtual void onExpire(); // Called when time to live expires
    virtual void generateMesh(); // Pure virtual
    // Bounding box methods
    virtual void updateBoundingBox() override;
    virtual glm::vec3 getMinBoundingBox() override;
    virtual glm::vec3 getMaxBoundingBox() override;

    // Utility methods
    virtual void clear() = 0; // Pure virtual

protected:
    // Helper methods
    bool checkCollisions(float deltaTime, glm::vec3 &outCollisionNormal);
};