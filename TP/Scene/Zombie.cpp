#include "Zombie.hpp"
#include <glm/gtx/transform.hpp>
#include "TP/Scene/World.hpp"
#include "TP/Character/Character.hpp"
#include "TP/Camera/Camera.hpp"

Zombie::Zombie(Transform transform, World* world, Camera* camera) : Entity() {
    m_world = world;
    this->camera = camera;
    
    m_transform = transform;
    
    generateZombieMesh(0.0f);

    this->setFPSActive(new bool(true));

    updateBoundingBox();

    targetPosition = transform.m_translation;
}

Zombie::~Zombie() {
    if (AABBRenderer != nullptr) {
        delete AABBRenderer;
    }
}

void Zombie::generateZombieMesh(float groundHeight) {
    generateHumanoidMesh(groundHeight);

    m_leftArm->rotate(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    m_rightArm->rotate(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    //il faut remonter les bras du zombe de la moitié de leur hauteur
    m_leftArm->translate(glm::vec3(0.f, (0.703125f-0.234375f)*0.5f, (0.703125f-0.234375f)*0.5f));
    m_rightArm->translate(glm::vec3(0.f, (0.703125f-0.234375f)*0.5f, (0.703125f-0.234375f)*0.5f));

    translate(glm::vec3(0.f, groundHeight, 0.f));

    
}

void Zombie::move(glm::vec3 direction) {
    translate(direction);
    updateBoundingBox();
}

void Zombie::update(float deltaTime) {
    updateTargetPosition(deltaTime);
    calculateMovementDirection(deltaTime);
    updateJumpCooldown(deltaTime);
    updateBoundingBox();
    updateRenderers();
    // std::cout<<"zombie isgrounded ?: "<<isGrounded<<std::endl;
}

void Zombie::updateTargetPosition(float deltaTime) {
    targetUpdateTimer += deltaTime;
    if (targetUpdateTimer >= targetUpdateInterval) {
        // Find a new target position (e.g., player position)
        SceneNode* root = getRoot();
        if (root) {
            for (auto child : root->getChildren()) {
                // Look for a Character instance among root's children
                if (typeid(*child) == typeid(Character)) {
                    Character* character = static_cast<Character*>(child);
                    targetPosition = character->getWorldPosition();
                    break;
                }
            }
        }
        
        targetUpdateTimer = 0.0f;
    }
}

void Zombie::calculateMovementDirection(float deltaTime) {
    glm::vec3 direction = targetPosition - getWorldPosition();
    direction.y = 0;
    
    if (glm::length(direction) > 0.1f) {
        direction = glm::normalize(direction) * moveSpeed * deltaTime;
        vecteurDirection = direction;
        
        detectAndHandleObstacles();
    } else {
        vecteurDirection = glm::vec3(0.0f);
    }
}

void Zombie::detectAndHandleObstacles() {
    if (!canJump) return;

    
    glm::vec3 position = getWorldPosition();
    glm::vec3 normalizedDir = glm::normalize(vecteurDirection);

    
    //on récupère le bloque devant la direction du zombie
    glm::vec3 checkPos = position + normalizedDir;
    int blockX = static_cast<int>(std::floor(checkPos.x));
    int blockY = static_cast<int>(std::floor(position.y));
    int blockZ = static_cast<int>(std::floor(checkPos.z));
    
    //si le bloc n'est pas de l'air ou de l'eau
    int blockType = m_world->getBloc(blockX, blockY, blockZ);
    if (blockType != AIR && blockType != WATER) {
        // jump();

        //on prend aussi le bloc au dessus car les humanoides font 2 de haut
        int blockAboveType = m_world->getBloc(blockX, blockY + 1, blockZ);
        int blockAbove2Type = m_world->getBloc(blockX, blockY + 2, blockZ);
        
        if (blockAboveType == AIR && blockAbove2Type == AIR) {
            //si on peux on saute

            if (velocity.y < 2.0f) {
                jump();
            }
        }
    }
}

void Zombie::jump() {
    velocity.y = jumpForce;
    canJump = false;
    jumpCooldown = 0.0f;
}

void Zombie::updateJumpCooldown(float deltaTime) {
    if (!canJump) {
        jumpCooldown += deltaTime;
        if (jumpCooldown >= jumpCooldownMax) {
            canJump = true;
        }
    }
}

void Zombie::updateRenderers() {
    if (AABBRenderer != nullptr) {
        AABBRenderer->setHighlight(getMinBoundingBox());
    }
}

void Zombie::updateBoundingBox() {
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

glm::vec3 Zombie::getMinBoundingBox() {
    glm::vec3 min = boundingBox[0];
    for (int i = 1; i < boundingBox.size(); ++i) {
        min = glm::min(min, boundingBox[i]);
    }
    return min;
}

glm::vec3 Zombie::getMaxBoundingBox() {
    glm::vec3 max = boundingBox[0];
    for (int i = 1; i < boundingBox.size(); ++i) {
        max = glm::max(max, boundingBox[i]);
    }
    return max;
}

void Zombie::drawBoundingBox() {
    if (!displayAABB || AABBRenderer == nullptr)
        return;
    
    AABBRenderer->drawWireframeCube(
        size,
        camera->getViewMatrix(),
        camera->getProjectionMatrix());
}

void Zombie::resolveGravity(float& deltaTime) {
    // Apply gravity based on context (water or not)
    if (isInWater) {
        // Reduced gravity in water
        velocity.y += -gravity * 0.3f * deltaTime;
        
        // Limit vertical speed in water
        velocity.y = glm::clamp(velocity.y, -4.0f, 2.0f);
    } else {
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
    for (int x = static_cast<int>(std::floor(minBB.x)); x <= static_cast<int>(std::floor(maxBB.x)); ++x) {
        for (int y = static_cast<int>(std::floor(minBB.y)); y <= static_cast<int>(std::floor(maxBB.y)); ++y) {
            for (int z = static_cast<int>(std::floor(minBB.z)); z <= static_cast<int>(std::floor(maxBB.z)); ++z) {
                int blockId = m_world->getBloc(x, y, z);
                
                if (blockId == WATER) {
                    isInWater = true;
                } else if (blockId != AIR) {
                    isGrounded = true;
                    break;
                }
            }
            if (isGrounded) break;
        }
        if (isGrounded) break;
    }
    
    // Apply position if not blocked
    if (!isGrounded) {
        translate(glm::vec3(0.f, velocity.y * deltaTime, 0.f));
        updateBoundingBox();
    } else {
        velocity.y = 0.0f;
    }
}