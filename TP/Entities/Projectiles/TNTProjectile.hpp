#pragma once

#include <glm/glm.hpp>
#include <TP/Scene/SceneNode.hpp>
#include <TP/Scene/World.hpp>

class TNTProjectile : public Entity
{
private:
    World* m_world;
    glm::vec3 m_velocity;
    float m_radius;
    float m_explosionRadius = TNT_EXPLOSION_RADIUS;
    float speed = TNT_PROJ_SPEED;
    std::shared_ptr<VoxelMeshObject> m_tntMesh;
    GLuint m_programID;

    glm::vec3 AABBmin;
    glm::vec3 AABBmax;
public:
    TNTProjectile(glm::vec3 position, glm::vec3 velocity, float radius, World* world, GLuint programID);
    ~TNTProjectile();

    void explode(int x, int y, int z);
    void generateTNTMesh();
    inline void setVelocity(glm::vec3 velocity) { m_velocity = velocity; }
    inline void setRadius(float radius) { m_radius = radius; }
    void draw(GLuint programID) override;
    void update(float deltaTime);
    void updateBoundingBox() override;
    glm::vec3 getMinBoundingBox() override;
    glm::vec3 getMaxBoundingBox() override;
    void clear();
};