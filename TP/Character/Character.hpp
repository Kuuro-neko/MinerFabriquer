#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include "utils/Transform.hpp"
#include "TP/Scene/MeshObject.hpp"
#include "TP/Scene/Texture.hpp"
#include "TP/Scene/SceneNode.hpp"
#include "TP/Camera/Camera.hpp"
#include "TP/Scene/VoxelChunk.hpp"
#include "Inventory.hpp"
#include "TP/Scene/Renderer.hpp"
#include <TP/Scene/World.hpp>

#define MAX_BREAK_COOLDOWN 0.4f
#define MAX_PLACE_COOLDOWN 0.4f

class Character : public SceneNode {

public:
    Character(Transform transform, Camera *camera, World* world = nullptr, MeshObject *mesh = nullptr, Texture *texture = nullptr);

    void rotateCharacter(float angle, glm::vec3 axis);
    inline void setRenderer(Renderer *renderer) { this->renderer = renderer; }
    void listenAction(float dt, GLFWwindow *window, BlocDatabase &database);
    void scrollCallback(GLFWwindow* window, double xOffset, double yOffset);
    void update(float dt) {
        if (breakCooldown < MAX_BREAK_COOLDOWN) {
            breakCooldown += dt;
        }
        if (placeCooldown < MAX_PLACE_COOLDOWN) {
            placeCooldown += dt;
        }
    }
    World* m_world;
    Camera *camera;
    Inventory *inventory;

private:
    void move(glm::vec3 direction);

    Renderer *renderer;
    void updateClosestBlock(BlocDatabase& database);
    void breakBlock(BlocDatabase &database);
    void putBlock(BlocDatabase &database);
    void setSelectedBlock(BlocDatabase &database);
    
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

    glm::vec3 blocPlusProche;
    int facePlusProche = -1;
    bool intersection = false;

    
};

#endif // CHARACTER_HPP