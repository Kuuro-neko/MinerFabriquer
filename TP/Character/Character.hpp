#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include "utils/Transform.hpp"
#include "TP/Scene/MeshObject.hpp"
#include "TP/Scene/Texture.hpp"
#include "TP/Scene/SceneNode.hpp"
#include "TP/Camera/Camera.hpp"
#include "TP/Scene/VoxelChunk.hpp"
#include "Inventory.hpp"
class Character : public SceneNode {

public:
    Character(Transform transform, Camera *camera, MeshObject *mesh, Texture *texture);

    void rotateCharacter(float angle, glm::vec3 axis);

    void listenAction(float key, GLFWwindow *window, VoxelChunk &chunkActuel, BlocDatabase &database);

    Camera *camera;
    Inventory *inventory;

private:
    void move(glm::vec3 direction);

    void breakBlock(VoxelChunk &chunkActuel, BlocDatabase &database) const;

    float speed;
    float maxInteractionDistance = 6.f;
};

#endif // CHARACTER_HPP