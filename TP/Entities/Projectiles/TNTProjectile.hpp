#pragma once

#include <glm/glm.hpp>
#include "Projectile.hpp"
#include <TP/Scene/World.hpp>
#include <memory>

class TNTProjectile : public Projectile
{
private:
    float m_explosionRadius = TNT_EXPLOSION_RADIUS;
    std::shared_ptr<VoxelMeshObject> m_mesh;

public:
    TNTProjectile(glm::vec3 position, glm::vec3 velocity, float radius, World *world, GLuint programID);
    virtual ~TNTProjectile() override;

    void explode(int x, int y, int z);
    void generateMesh() override;

    // Overridden methods from Projectile
    void draw(GLuint programID) override;
    void onExpire() override;
    void clear() override;
    // Deliberately NOT overriding onCollision to preserve bouncing behavior
};