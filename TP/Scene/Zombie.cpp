#include "Zombie.hpp"
#include <glm/gtx/transform.hpp>

Zombie::Zombie(const Transform& transform, World* world) : Entity() {
    m_world = world;
    
    m_transform = transform;
    
    generateZombieMesh(0.0f);

    this->setFPSActive(new bool(true));
}

void Zombie::generateZombieMesh(float groundHeight) {
    generateHumanoidMesh(groundHeight);

    m_leftArm->rotate(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    m_rightArm->rotate(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    //il faut remonter les bras du zombe de la moitié de leur hauteur
    m_leftArm->translate(glm::vec3(0.f, (0.703125f-0.234375f)*0.5f, (0.703125f-0.234375f)*0.5f));
    m_rightArm->translate(glm::vec3(0.f, (0.703125f-0.234375f)*0.5f, (0.703125f-0.234375f)*0.5f));

    
}