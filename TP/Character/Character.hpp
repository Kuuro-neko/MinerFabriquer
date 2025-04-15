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
#include "vector"

#define MAX_BREAK_COOLDOWN 0.4f
#define MAX_PLACE_COOLDOWN 0.4f

class Character : public SceneNode {

public:
    Character(Transform transform, Camera *camera, World* world = nullptr, MeshObject *mesh = nullptr, Texture *texture = nullptr);

    inline void setRenderer(Renderer *renderer) { this->renderer = renderer; }
    void listenAction(float dt, GLFWwindow *window, BlocDatabase &database);
    void scrollCallback(GLFWwindow* window, double xOffset, double yOffset);
    void update(float dt);
    void updateBoundingBox();
    World* m_world;
    Camera *camera;
    Inventory *inventory;

    glm::vec3 velocity = glm::vec3(0.f);

    glm::vec3 getMinBoundingBox();

    glm::vec3 getMaxBoundingBox();

    inline void applyGravity() {
        translate(glm::vec3(0.f, -gravity, 0.f));
    }

private:
    void move(glm::vec3 direction);
    void updateClosestBlock(BlocDatabase& database);
    void breakBlock(BlocDatabase &database);
    void putBlock(BlocDatabase &database);
    void setSelectedBlock(BlocDatabase &database);
    inline void resetBreakCooldown() {
        breakCooldown = 0.f;
    }
    inline void resetPlaceCooldown() {
        placeCooldown = 0.f;
    }

    Renderer *renderer;
    float speed = 2.5f;
    float maxInteractionDistance = 6.f;
    float gravity = 9.81f;
    float breakCooldown = std::numeric_limits<float>::max();
    float placeCooldown = std::numeric_limits<float>::max();

    std::vector<glm::vec3> boundingBox;
    float size = 0.5;
    // Minecraft AABB width : 5/8 
    // Minecraft AABB height : 29/32
    // Minecraft AABB height while sneaking : 1.5

    glm::vec3 blocPlusProche;

    int facePlusProche = -1;

    bool intersection = false;



    glm::vec3 getSize();


    //TODO LIST

    //1) récupérer bounding box joueur

    // a chaque tic on met à jour la velocité du joueur

    //2) mettre à jour le bounding box du joueur à chaque tic
    //3) récupérer dans quel chunk se trouve le joueur
    //4)


};

#endif // CHARACTER_HPP