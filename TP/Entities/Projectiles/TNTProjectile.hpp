#pragma once

#include <glm/glm.hpp>
#include "Projectile.hpp"
#include <TP/Scene/World.hpp>
#include <memory>

// Assuming these constants are defined elsewhere
#ifndef TNT_EXPLOSION_RADIUS
#define TNT_EXPLOSION_RADIUS 3.0f
#endif

#ifndef TNT_PROJ_SPEED
#define TNT_PROJ_SPEED 10.0f
#endif

class TNTProjectile : public Projectile
{
private:
    float m_explosionRadius = TNT_EXPLOSION_RADIUS;
    std::shared_ptr<VoxelMeshObject> m_tntMesh;

public:
    TNTProjectile(glm::vec3 position, glm::vec3 velocity, float radius, World *world, GLuint programID);
    virtual ~TNTProjectile() override;

    void explode(int x, int y, int z);
    void generateTNTMesh();

    // Overridden methods from Projectile
    void draw(GLuint programID) override;
    void onExpire() override;
    void clear() override;
};