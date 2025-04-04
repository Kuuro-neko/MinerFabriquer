#include "Character.hpp"

#include <iostream>

using namespace std;

Character::Character(Transform transform, Camera camera, MeshObject *mesh = nullptr, Texture *texture = nullptr)
        : SceneNode(transform, mesh, texture), camera(camera) {
    speed = 1.0f;
}

void Character::move(glm::vec3 direction) {
    translate(direction*speed);
}