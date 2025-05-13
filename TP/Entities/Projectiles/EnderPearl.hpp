#pragma once
#include <glm/glm.hpp>
#include "Projectile.hpp"
#include <TP/Scene/World.hpp>
#include <memory>
#include <TP/Character/Character.hpp>

class EnderPearl : public Projectile
{
private:
    std::shared_ptr<VoxelMeshObject> m_enderPearlMesh;
    Character* m_character;

public:
    EnderPearl(Character* character, glm::vec3 velocity, float radius, World *world, GLuint programID);
    virtual ~EnderPearl() override;

    void teleport(int x, int y, int z);
    void generateMesh() override;

    // Overridden methods from Projectile
    void draw(GLuint programID) override;
    void onExpire() override;
    void clear() override;
    void onCollision(const glm::vec3 &collisionNormal) override;

};