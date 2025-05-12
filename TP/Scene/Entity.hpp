#pragma once

#include <TP/Scene/SceneNode.hpp>
#include <TP/Scene/MeshObject.hpp>
#include <TP/Textures/Texture.hpp>
#include <utils/Transform.hpp>
#include <map>
#include <string>
#include <vector>
#include <glm/gtx/quaternion.hpp>

// Forward declarations
class World;
class Camera;

/**
 * Base class for all entities in the game world
 */
class Entity : public SceneNode
{
public:
    Entity();
    virtual ~Entity();

    // Core entity methods
    virtual void update(float &deltaTime) {};
    virtual void resolveGravity(float &deltaTime) {};
    virtual void resolveCollisions(float &deltaTime) {};
    virtual void move(const glm::vec3 &direction) {};

    // Bounding box methods
    virtual void updateBoundingBox() {};
    virtual void drawBoundingBox() {};
    virtual glm::vec3 getMinBoundingBox() { return glm::vec3(0.0f); }
    virtual glm::vec3 getMaxBoundingBox() { return glm::vec3(0.0f); }
    virtual void setDisplayAABB(bool display) {};

    // Texture handling
    virtual void setTexture(PBRTexture *texture);
    PBRTexture *getTexture() const { return m_pbr_texture; }

    // Draw methods
    void draw(GLuint programID) override;
    void cleanupBuffers() override;

    // FPS mode handling
    inline void setIsFPS(bool *FPSActive) { this->FPSActive = FPSActive; }
    void setFPSActive(bool *attached);
    bool isFPSActive();
    
    PBRTexture* m_pbr_texture = nullptr;
protected:
    bool *FPSActive = nullptr;
    World *m_world = nullptr;
};