#ifndef ZOMBIE_HPP
#define ZOMBIE_HPP

#include "TP/Scene/Entity.hpp"
#include "TP/Scene/World.hpp"
#include <Defines.hpp>

class Zombie : public Entity {
public:
    Zombie(const Transform& transform, World* world);
    void generateZombieMesh(float groundHeight);

private:
    World* m_world;
};

#endif