#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include "utils/Transform.hpp"
#include "TP/Scene/MeshObject.hpp"
#include "TP/Scene/Texture.hpp"
#include "TP/Scene/SceneNode.hpp"
#include "TP/Camera/Camera.hpp"
#include "TP/Scene/VoxelChunk.hpp"
#include "Inventory.hpp"

#define MAX_BREAK_COOLDOWN 0.1f
#define MAX_PLACE_COOLDOWN 0.15f

class Character : public SceneNode {

public:
    Character(Transform transform, Camera *camera, MeshObject *mesh, Texture *texture);

    void rotateCharacter(float angle, glm::vec3 axis);

    void listenAction(float key, GLFWwindow *window, VoxelChunk &chunkActuel, BlocDatabase &database);
    void scrollCallback(GLFWwindow* window, double xOffset, double yOffset);
    void update(float dt) {
        if (breakCooldown < MAX_BREAK_COOLDOWN) {
            breakCooldown += dt;
        }
        if (placeCooldown < MAX_PLACE_COOLDOWN) {
            placeCooldown += dt;
        }
    }

    Camera *camera;
    Inventory *inventory;

private:
    void move(glm::vec3 direction);

    void breakBlock(VoxelChunk &chunkActuel, BlocDatabase &database);
    void putBlock(VoxelChunk &chunkActuel, BlocDatabase &database);
    void resetBreakCooldown() {
        breakCooldown = 0.f;
    }
    void resetPlaceCooldown() {
        placeCooldown = 0.f;
    }

    float speed;
    float maxInteractionDistance = 6.f;
    float breakCooldown = std::numeric_limits<float>::max();
    float placeCooldown = std::numeric_limits<float>::max();
};

#endif // CHARACTER_HPP