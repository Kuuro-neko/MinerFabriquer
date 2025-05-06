#ifndef ZOMBIE_HPP
#define ZOMBIE_HPP

#include "TP/Scene/Entity.hpp"
#include <Defines.hpp>
#include "TP/Scene/Renderer.hpp"
class World;
class Camera;

class Zombie : public Entity {
public:
    Zombie(Transform transform, World* world, Camera* camera);
    ~Zombie();

    void resolveGravity(float& deltaTime);
    void update(float deltaTime);
    void move(glm::vec3 direction);
    void generateZombieMesh(float groundHeight);
    // New methods
    void updateBoundingBox();
    void drawBoundingBox();
    
    // Updated methods to match Character class
    glm::vec3 getMinBoundingBox();
    glm::vec3 getMaxBoundingBox();

    // Public variables for collision resolution
    glm::vec3 velocity = glm::vec3(0.0f);
    glm::vec3 vecteurDirection = glm::vec3(0.0f);
    bool isInWater = false;
    Renderer* AABBRenderer = nullptr;


    // Set wireframe renderer for bounding box visualization
    inline void setWireframeRenderer(GLuint wireframeProgramID) {
        this->AABBRenderer = new Renderer(wireframeProgramID);
    }

    inline void setDisplayAABB(bool display) {
        this->displayAABB = display;
    }

private:
    World* m_world;
    Camera* camera;
    std::vector<glm::vec3> boundingBox;
    glm::vec3 size = glm::vec3(5.f / 8.f, 29.f / 16.f, 5.f / 8.f); // Similar to character size
    bool isGrounded = false;
    float gravity = -9.81f;
    float moveSpeed = 1.0f;
    float targetUpdateTimer = 0.0f;
    float targetUpdateInterval = 2.0f;
    glm::vec3 targetPosition;
    bool displayAABB = false; // Debug flag for rendering the bounding box
    
};

#endif