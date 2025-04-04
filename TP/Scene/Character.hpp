#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include "utils/Transform.hpp"
#include "MeshObject.hpp"
#include "Texture.hpp"


class Character {
public:
    Character(Transform transform, MeshObject* mesh, Texture* texture);
    void move(glm::vec3 direction, float amount);
    void rotateCharacter(float angle, glm::vec3 axis);
private:
    Transform transform;
    MeshObject* mesh;
    Texture* texture;
};

#endif // CHARACTER_HPP