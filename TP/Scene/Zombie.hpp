#ifndef ZOMBIE_HPP
#define ZOMBIE_HPP

#include "HumanoidEntity.hpp"
#include <Defines.hpp>
#include "TP/Scene/Renderer.hpp"

class World;
class Camera;

enum ZombieState
{
    ZOMBIE_IDLE = IDLE,
    ZOMBIE_PURSUIT = WALKING,
    ZOMBIE_ATTACK = ATTACKING
};

class Zombie : public HumanoidEntity
{
public:
    Zombie(Transform transform, World *world, Camera *camera);
    ~Zombie() override;

    // Override entity methods
    void resolveGravity(float &deltaTime) override;
    void move(const glm::vec3 &direction) override;
    void update(float &deltaTime) override;
    void updateBoundingBox() override;
    void drawBoundingBox() override;

    // Specialized zombie methods
    void generateZombieMesh(float &groundHeight);

    // Bounding box getters
    glm::vec3 getMinBoundingBox() override;
    glm::vec3 getMaxBoundingBox() override;

    // State management
    ZombieState getState() const { return static_cast<ZombieState>(m_currentState); }
    void setState(ZombieState newState);

    // Rendering options
    inline void setWireframeRenderer(GLuint wireframeProgramID)
    {
        this->AABBRenderer = new Renderer(wireframeProgramID);
    }

    inline void setDisplayAABB(bool display) override
    {
        this->displayAABB = display;
    }
    glm::vec3 vecteurDirection = glm::vec3(0.0f);

private:
    // Movement and physics properties
    glm::vec3 velocity = glm::vec3(0.0f);
    glm::vec3 size = glm::vec3(5.f / 8.f, 29.f / 16.f, 5.f / 8.f);
    bool isInWater = false;
    bool isGrounded = false;
    float gravity = -9.81f;
    float moveSpeed = 1.0f;

    // Target tracking
    Camera *m_camera;
    float targetUpdateTimer = 0.0f;
    float targetUpdateInterval = 2.0f;
    glm::vec3 targetPosition;
    glm::vec3 directionToTarget;
    float distanceToTarget;
    float currentRotationAngle = 0.0f;
    float rotationSpeed = 2.0f;

    // Jumping
    float jumpCooldown = 0.0f;
    float jumpCooldownMax = 1.0f;
    bool canJump = true;
    float jumpForce = 5.0f;

    // Attack stats
    float attackCooldown = 0.0f;
    float attackCooldownMax = 1.0f;
    float attackRange = 1.0f;
    float attackDamage = 1.0f;

    // Bounding box
    std::vector<glm::vec3> boundingBox;
    Renderer *AABBRenderer = nullptr;
    bool displayAABB = false;

    // State management
    ZombieState currentState = ZOMBIE_IDLE;

    // Private methods
    void initializeZombieAnimations();
    void createIdlePoses();
    void createWalkingPoses();
    void createAttackPoses();

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
    void faceTarget(glm::vec3 targetPos, float &deltaTime);
};

#endif