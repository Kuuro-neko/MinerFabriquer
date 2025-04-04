#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include "utils/Transform.hpp"
#include "MeshObject.hpp"
#include "Texture.hpp"
#include "SceneNode.hpp"
#include "TP/Camera/Camera.hpp"


class Character  : public SceneNode {

public:
    Character(Transform transform,Camera camera, MeshObject* mesh, Texture* texture);

    void rotateCharacter(float angle, glm::vec3 axis);
    void listenAction(float key, GLFWwindow *window);
    Camera camera;
private:
    void move(glm::vec3 direction);
    void breakBlock();
    float speed;
};

#endif // CHARACTER_HPP