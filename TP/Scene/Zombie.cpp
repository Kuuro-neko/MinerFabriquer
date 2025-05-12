#include "Zombie.hpp"
#include <glm/gtx/transform.hpp>
#include "TP/Scene/World.hpp"
#include "TP/Character/Character.hpp"
#include "TP/Camera/Camera.hpp"

#ifndef ZOMBIE_DISTANCE_FINDING_PLAYER
#define ZOMBIE_DISTANCE_FINDING_PLAYER 15.0f
#endif

Zombie::Zombie(Transform transform, World *world, Camera *camera) : HumanoidEntity()
{
    m_world = world;
    m_camera = camera;
    m_transform = transform;
    float groundHeight = 0.0F;
    generateZombieMesh(groundHeight);
    initializeZombieAnimations();
    setState(ZOMBIE_IDLE);
    this->setFPSActive(new bool(true));
    updateBoundingBox();
    currentState = ZOMBIE_IDLE;
    targetPosition = transform.m_translation;
    PBRTexture* zombieTexture = new PBRTexture("../textures/zombie/zombie.png",
        "../textures/zombie/zombie_normal.png",
        "../textures/zombie/zombie_roughness.png",
        "../textures/zombie/zombie_metallic.png");
    Entity::setTexture(zombieTexture);
}

Zombie::~Zombie()
{
    if (AABBRenderer != nullptr)
    {
        delete AABBRenderer;
    }
}

void Zombie::generateZombieMesh(float &groundHeight)
{
    generateHumanoidMesh(groundHeight);

    // Position arms in zombie pose
    m_leftArm->rotate(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    m_rightArm->rotate(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    // Adjust arms position
    m_leftArm->translate(glm::vec3(0.f, (0.703125f - 0.234375f) * 0.5f, (0.703125f - 0.234375f) * 0.5f));
    m_rightArm->translate(glm::vec3(0.f, (0.703125f - 0.234375f) * 0.5f, (0.703125f - 0.234375f) * 0.5f));
}

void Zombie::move(const glm::vec3 &direction)
{
    translate(direction);
    updateBoundingBox();
}

void Zombie::update(float &deltaTime)
{
    updateState(deltaTime);

    switch (currentState)
    {
    case ZOMBIE_IDLE:
        handleIdleState(deltaTime);
        break;

    case ZOMBIE_PURSUIT:
        handlePursuitState(deltaTime);
        break;

    case ZOMBIE_ATTACK:
        handleAttackState(deltaTime);
        break;
    }

    HumanoidEntity::updateAnimation(deltaTime);
    updateJumpCooldown(deltaTime);
    updateBoundingBox();
    updateRenderers();
}

void Zombie::updateState(float deltaTime)
{
    // Find player
    Character *player = nullptr;
    SceneNode *root = getRoot();

    if (root)
    {
        for (auto child : root->getChildren())
        {
            if (typeid(*child) == typeid(Character))
            {
                player = static_cast<Character *>(child);
                break;
            }
        }
    }

    if (!player)
    {
        setState(ZOMBIE_IDLE);
        return;
    }

    targetPosition = player->getWorldPosition();
    directionToTarget = targetPosition - getWorldPosition();
    distanceToTarget = glm::length(directionToTarget);

    if (distanceToTarget <= attackRange)
    {
        setState(ZOMBIE_ATTACK);
    }
    else if (distanceToTarget <= ZOMBIE_DISTANCE_FINDING_PLAYER)
    {
        setState(ZOMBIE_PURSUIT);
    }
    else
    {
        setState(ZOMBIE_IDLE);
    }
}

void Zombie::handleIdleState(float deltaTime)
{
    vecteurDirection = glm::vec3(0.0f);
}

void Zombie::handlePursuitState(float deltaTime)
{
    updateTargetPosition(deltaTime);
    faceTarget(targetPosition, deltaTime);
    calculateMovementDirection(deltaTime);
}

void Zombie::updateTargetPosition(float deltaTime)
{
    targetUpdateTimer += deltaTime;
    if (targetUpdateTimer >= targetUpdateInterval)
    {
        // Find a new target position (e.g., player position)
        SceneNode *root = getRoot();
        if (root)
        {
            for (auto child : root->getChildren())
            {
                // Look for a Character instance among root's children
                if (typeid(*child) == typeid(Character))
                {
                    Character *character = static_cast<Character *>(child);
                    targetPosition = character->getWorldPosition();
                    break;
                }
            }
        }

        targetUpdateTimer = 0.0f;
    }
}

void Zombie::calculateMovementDirection(float deltaTime)
{
    glm::vec3 horizontalDir = directionToTarget;
    horizontalDir.y = 0;

    if (glm::length(horizontalDir) > 0.1f)
    {
        horizontalDir = glm::normalize(horizontalDir) * moveSpeed * deltaTime;
        vecteurDirection = horizontalDir;

        detectAndHandleObstacles();
    }
    else
    {
        vecteurDirection = glm::vec3(0.0f);
    }
}

void Zombie::detectAndHandleObstacles()
{
    if (!canJump)
        return;

    glm::vec3 position = getWorldPosition();
    glm::vec3 normalizedDir = glm::normalize(vecteurDirection);

    // Check block in front of the zombie's direction
    glm::vec3 checkPos = position + normalizedDir;
    int blockX = static_cast<int>(std::floor(checkPos.x));
    int blockY = static_cast<int>(std::floor(position.y));
    int blockZ = static_cast<int>(std::floor(checkPos.z));

    // If the block is not air or water
    int blockType = m_world->getBloc(blockX, blockY, blockZ);
    if (blockType != AIR && blockType != WATER)
    {
        // Check if there's room to jump (2 blocks high)
        int blockAboveType = m_world->getBloc(blockX, blockY + 1, blockZ);
        int blockAbove2Type = m_world->getBloc(blockX, blockY + 2, blockZ);

        if (blockAboveType == AIR && blockAbove2Type == AIR)
        {
            // If we can, we jump
            if (velocity.y < 2.0f)
            {
                jump();
            }
        }
    }
}

void Zombie::jump()
{
    velocity.y = jumpForce;
    canJump = false;
    jumpCooldown = 0.0f;
}

void Zombie::updateJumpCooldown(float deltaTime)
{
    if (!canJump)
    {
        jumpCooldown += deltaTime;
        if (jumpCooldown >= jumpCooldownMax)
        {
            canJump = true;
        }
    }
}

void Zombie::handleAttackState(float deltaTime)
{
    faceTarget(targetPosition, deltaTime);
    attackCooldown += deltaTime;

    if (attackCooldown >= attackCooldownMax)
    {
        attackCooldown = 0.0f;
    }
}

void Zombie::updateRenderers()
{
    if (AABBRenderer != nullptr)
    {
        AABBRenderer->setHighlight(getMinBoundingBox());
    }
}

void Zombie::updateBoundingBox()
{
    boundingBox.clear();
    glm::vec3 position = getWorldPosition();
    boundingBox.push_back(position + glm::vec3(-size.x / 2, -size.y / 2, -size.z / 2));
    boundingBox.push_back(position + glm::vec3(size.x / 2, -size.y / 2, -size.z / 2));
    boundingBox.push_back(position + glm::vec3(size.x / 2, size.y / 2, -size.z / 2));
    boundingBox.push_back(position + glm::vec3(-size.x / 2, size.y / 2, -size.z / 2));
    boundingBox.push_back(position + glm::vec3(-size.x / 2, -size.y / 2, size.z / 2));
    boundingBox.push_back(position + glm::vec3(size.x / 2, -size.y / 2, size.z / 2));
    boundingBox.push_back(position + glm::vec3(size.x / 2, size.y / 2, size.z / 2));
    boundingBox.push_back(position + glm::vec3(-size.x / 2, size.y / 2, size.z / 2));
}

glm::vec3 Zombie::getMinBoundingBox()
{
    glm::vec3 min = boundingBox[0];
    for (size_t i = 1; i < boundingBox.size(); ++i)
    {
        min = glm::min(min, boundingBox[i]);
    }
    return min;
}

glm::vec3 Zombie::getMaxBoundingBox()
{
    glm::vec3 max = boundingBox[0];
    for (size_t i = 1; i < boundingBox.size(); ++i)
    {
        max = glm::max(max, boundingBox[i]);
    }
    return max;
}

void Zombie::drawBoundingBox()
{
    if (!displayAABB || AABBRenderer == nullptr)
        return;

    AABBRenderer->drawWireframeCube(
        size,
        m_camera->getViewMatrix(),
        m_camera->getProjectionMatrix());
}

void Zombie::resolveGravity(float &deltaTime)
{
    // Apply gravity based on context (water or not)
    if (isInWater)
    {
        // Reduced gravity in water
        velocity.y += -gravity * 0.3f * deltaTime;

        // Limit vertical speed in water
        velocity.y = glm::clamp(velocity.y, -4.0f, 2.0f);
    }
    else
    {
        // Normal gravity outside water
        velocity.y += gravity * deltaTime;
    }

    // Predict next position
    glm::vec3 nextPosition = getWorldPosition() + velocity * deltaTime;

    // Update bounding box with vertical velocity
    updateBoundingBox();
    glm::vec3 minBB = getMinBoundingBox();
    glm::vec3 maxBB = getMaxBoundingBox();

    minBB.y += velocity.y * deltaTime;
    maxBB.y += velocity.y * deltaTime;

    // Reset state
    isGrounded = false;
    isInWater = false;

    // Check collisions within bounding box volume
    for (int x = static_cast<int>(std::floor(minBB.x)); x <= static_cast<int>(std::floor(maxBB.x)); ++x)
    {
        for (int y = static_cast<int>(std::floor(minBB.y)); y <= static_cast<int>(std::floor(maxBB.y)); ++y)
        {
            for (int z = static_cast<int>(std::floor(minBB.z)); z <= static_cast<int>(std::floor(maxBB.z)); ++z)
            {
                int blockId = m_world->getBloc(x, y, z);

                if (blockId == WATER)
                {
                    isInWater = true;
                }
                else if (blockId != AIR)
                {
                    isGrounded = true;
                    break;
                }
            }
            if (isGrounded)
                break;
        }
        if (isGrounded)
            break;
    }

    // Apply position if not blocked
    if (!isGrounded)
    {
        translate(glm::vec3(0.f, velocity.y * deltaTime, 0.f));
        updateBoundingBox();
    }
    else
    {
        velocity.y = 0.0f;
    }
}

void Zombie::draw(GLuint programID)
{
    Entity::draw(programID);
}

void Zombie::setState(ZombieState newState)
{
    if (newState != currentState) {
        currentState = newState;
        HumanoidEntity::setState(static_cast<EntityState>(newState));
    }
}

void Zombie::faceTarget(glm::vec3 targetPos, float &deltaTime)
{
    glm::vec3 horizontalDir = directionToTarget;
    horizontalDir.y = 0;
    if (glm::length(horizontalDir) > 0.001f)
    {
        horizontalDir = glm::normalize(horizontalDir);

        float targetAngle = atan2(horizontalDir.x, horizontalDir.z);
        float angleDiff = targetAngle - currentRotationAngle;

        // Interpolation:
        float step = rotationSpeed * deltaTime;

        // Keep angle between -pi and pi to avoid 360° rotations
        while (angleDiff > M_PI)
            angleDiff -= 2.0f * M_PI;
        while (angleDiff < -M_PI)
            angleDiff += 2.0f * M_PI;

        if (abs(angleDiff) < step)
        {
            currentRotationAngle = targetAngle;
        }
        else
        {
            currentRotationAngle += (angleDiff > 0) ? step : -step;
        }

        m_transform.m_rotation = DEFAULT_ROTATION;
        rotate(currentRotationAngle, AXIS_Y);
    }
}

void Zombie::initializeZombieAnimations()
{
    // Initialize base pose from current state
    initializeBasePose();

    // Create zombie-specific poses
    createIdlePoses();
    createWalkingPoses();
    createAttackPoses();

    // Create animation sequences
    std::vector<std::string> idlePoses = {"base", "idle_sway", "base"};
    std::vector<float> idleDurations = {1.0f, 1.0f, 1.0f};
    createAnimationSequence("idle", idlePoses, idleDurations, true);

    std::vector<std::string> walkPoses = {"walk_left", "base", "walk_right", "base"};
    std::vector<float> walkDurations = {0.3f, 0.15f, 0.3f, 0.15f};
    createAnimationSequence("walking", walkPoses, walkDurations, true);

    std::vector<std::string> attackPoses = {"attack_prepare", "attack_strike", "base"};
    std::vector<float> attackDurations = {0.2f, 0.1f, 0.4f};
    createAnimationSequence("attacking", attackPoses, attackDurations, false);

    // Set default sequence
    setCurrentSequence("idle");
}

void Zombie::createIdlePoses()
{
    EntityPose &basePose = m_poses["base"];

    EntityPose idleSway("idle_sway");
    idleSway = basePose;

    // Left arm
    glm::mat3x3 leftArmRot = idleSway.leftArmTransform.m_rotation;
    glm::mat3x3 rotMatZ = glm::mat3x3(glm::rotate(glm::mat4(1.0f), glm::radians(-10.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    idleSway.leftArmTransform.m_rotation = rotMatZ * leftArmRot;

    // Right arm
    glm::mat3x3 rightArmRot = idleSway.rightArmTransform.m_rotation;
    rotMatZ = glm::mat3x3(glm::rotate(glm::mat4(1.0f), glm::radians(10.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    idleSway.rightArmTransform.m_rotation = rotMatZ * rightArmRot;

    addPose("idle_sway", idleSway);
}

void Zombie::createWalkingPoses()
{
    EntityPose &basePose = m_poses["base"];

    // Walking pose - left leg forward
    EntityPose walkLeft("walk_left");
    walkLeft = basePose;

    // Rotate left leg 30° forward
    glm::mat3x3 rotMatX = glm::mat3x3(glm::rotate(glm::mat4(1.0f), glm::radians(-30.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
    walkLeft.leftLegTransform.m_rotation = rotMatX * walkLeft.leftLegTransform.m_rotation;

    // Rotate right leg 15° backward
    rotMatX = glm::mat3x3(glm::rotate(glm::mat4(1.0f), glm::radians(15.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
    walkLeft.rightLegTransform.m_rotation = rotMatX * walkLeft.rightLegTransform.m_rotation;

    // Left arm at 70° (instead of default 90°)
    rotMatX = glm::mat3x3(glm::rotate(glm::mat4(1.0f), glm::radians(-70.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
    walkLeft.leftArmTransform.m_rotation = rotMatX;

    // Right arm at 110° (instead of default 90°)
    rotMatX = glm::mat3x3(glm::rotate(glm::mat4(1.0f), glm::radians(-110.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
    walkLeft.rightArmTransform.m_rotation = rotMatX;

    addPose("walk_left", walkLeft);

    // Walking pose - right leg forward
    EntityPose walkRight("walk_right");
    walkRight = basePose;

    // Mirror of walk_left
    rotMatX = glm::mat3x3(glm::rotate(glm::mat4(1.0f), glm::radians(15.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
    walkRight.leftLegTransform.m_rotation = rotMatX * walkRight.leftLegTransform.m_rotation;

    rotMatX = glm::mat3x3(glm::rotate(glm::mat4(1.0f), glm::radians(-30.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
    walkRight.rightLegTransform.m_rotation = rotMatX * walkRight.rightLegTransform.m_rotation;

    rotMatX = glm::mat3x3(glm::rotate(glm::mat4(1.0f), glm::radians(-110.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
    walkRight.leftArmTransform.m_rotation = rotMatX;

    rotMatX = glm::mat3x3(glm::rotate(glm::mat4(1.0f), glm::radians(-70.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
    walkRight.rightArmTransform.m_rotation = rotMatX;

    addPose("walk_right", walkRight);
}

void Zombie::createAttackPoses()
{
    EntityPose &basePose = m_poses["base"];

    // Attack pose - preparation
    EntityPose attackPrepare("attack_prepare");
    attackPrepare = basePose;

    // Arms back to prepare attack
    glm::mat3x3 rotMatX = glm::mat3x3(glm::rotate(glm::mat4(1.0f), glm::radians(-60.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
    attackPrepare.leftArmTransform.m_rotation = rotMatX;
    attackPrepare.rightArmTransform.m_rotation = rotMatX;

    addPose("attack_prepare", attackPrepare);

    // Attack pose - strike
    EntityPose attackStrike("attack_strike");
    attackStrike = basePose;

    // Arms forward for strike
    rotMatX = glm::mat3x3(glm::rotate(glm::mat4(1.0f), glm::radians(-140.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
    attackStrike.leftArmTransform.m_rotation = rotMatX;
    attackStrike.rightArmTransform.m_rotation = rotMatX;

    addPose("attack_strike", attackStrike);
}