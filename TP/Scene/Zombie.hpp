#ifndef ZOMBIE_HPP
#define ZOMBIE_HPP

#include "TP/Scene/Entity.hpp"
#include <Defines.hpp>
#include "TP/Scene/Renderer.hpp"
class World;
class Camera;

enum ZombieState {
    ZOMBIE_IDLE,
    ZOMBIE_PURSUIT,
    ZOMBIE_ATTACK
};

class Zombie : public Entity {
public:
    Zombie(Transform transform, World* world, Camera* camera);
    ~Zombie();

    void resolveGravity(float& deltaTime);
    void update(float deltaTime);
    void move(glm::vec3 direction);
    void generateZombieMesh(float groundHeight);
    void updateBoundingBox();
    void drawBoundingBox();
    
    
    glm::vec3 getMinBoundingBox();
    glm::vec3 getMaxBoundingBox();

    glm::vec3 velocity = glm::vec3(0.0f);
    glm::vec3 vecteurDirection = glm::vec3(0.0f);
    bool isInWater = false;
    Renderer* AABBRenderer = nullptr;


    inline void setWireframeRenderer(GLuint wireframeProgramID) {
        this->AABBRenderer = new Renderer(wireframeProgramID);
    }

    inline void setDisplayAABB(bool display) {
        this->displayAABB = display;
    }

    ZombieState getState() const { return currentState; }
    void setState(ZombieState newState);
    ZombieState currentState = ZOMBIE_IDLE;

private:
    World* m_world;
    Camera* camera;
    std::vector<glm::vec3> boundingBox;
    glm::vec3 size = glm::vec3(5.f / 8.f, 29.f / 16.f, 5.f / 8.f); 
    bool isGrounded = false;
    float gravity = -9.81f;
    float moveSpeed = 1.0f;
    float targetUpdateTimer = 0.0f;
    float targetUpdateInterval = 2.0f;
    glm::vec3 targetPosition;
    bool displayAABB = false; 


    float jumpCooldown = 0.0f;
    float jumpCooldownMax = 1.0f;
    bool canJump = true;
    float jumpForce = 5.0f;


    float attackCooldown = 0.0f;
    float attackCooldownMax = 1.0f;
    float attackRange = 1.0f;
    float attackDamage = 1.0f;


    void updateTargetPosition(float deltaTime);
    void calculateMovementDirection(float deltaTime);
    void detectAndHandleObstacles();
    void jump();
    void updateJumpCooldown(float deltaTime);
    void updateRenderers();
    
    void updateState(float deltaTime);
    void handleIdleState(float deltaTime);
    void handlePursuitState(float deltaTime);
    void handleAttackState(float deltaTime);

    void faceTarget(glm::vec3 targetPos, float& deltaTime);

    float currentRotationAngle = 0.0f;
    float rotationSpeed = 2.0f;
    
};

#endif