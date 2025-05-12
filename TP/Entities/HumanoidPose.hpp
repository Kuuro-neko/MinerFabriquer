#pragma once
#include "Entity.hpp"
#include <glm/gtx/quaternion.hpp>

// Forward declarations
struct EntityPose;
struct AnimationSequence;

enum EntityState
{
    IDLE,
    WALKING,
    ATTACKING
};

/**
 * Structure for entity poses used in animations
 */
struct EntityPose
{
    std::string name;

    Transform headTransform;
    Transform torsoTransform;
    Transform leftArmTransform;
    Transform rightArmTransform;
    Transform leftLegTransform;
    Transform rightLegTransform;

    EntityPose(std::string poseName = "") : name(poseName) {}
    void initFromEntity(HumanoidEntity *entity);
};

/**
 * Structure for animation sequences
 */
struct AnimationSequence
{
    std::vector<EntityPose> keyPoses;
    std::vector<float> durations;
    bool loop;

    AnimationSequence() : loop(true) {}
};

/**
 * Base class for all humanoid entities (with head, arms, legs)
 */
class HumanoidEntity : public Entity
{
public:
    HumanoidEntity();
    virtual ~HumanoidEntity();

    // Body part accessors
    SceneNode *getHead() { return m_head; }
    SceneNode *getLeftArm() { return m_leftArm; }
    SceneNode *getRightArm() { return m_rightArm; }
    SceneNode *getLeftLeg() { return m_leftLeg; }
    SceneNode *getRightLeg() { return m_rightLeg; }

    // Mesh setters
    void setHeadMesh(MeshObject *mesh);
    void setTorsoMesh(MeshObject *mesh);
    void setLeftArmMesh(MeshObject *mesh);
    void setRightArmMesh(MeshObject *mesh);
    void setLeftLegMesh(MeshObject *mesh);
    void setRightLegMesh(MeshObject *mesh);

    // Override entity methods
    void cleanupBuffers() override;

    // Humanoid mesh generation
    virtual void generateHumanoidMesh(float groundHeight);

    // Animation system
    virtual void setState(EntityState newState);
    virtual void updateAnimation(float deltaTime);

    // Animation setup
    void initializeBasePose();
    void addPose(const std::string &name, const EntityPose &pose);
    void createAnimationSequence(const std::string &name,
                                 const std::vector<std::string> &poseNames,
                                 const std::vector<float> &durations,
                                 bool loop = true);
    void setCurrentSequence(const std::string &sequenceName);

protected:
    // Body parts as scene nodes
    SceneNode *m_head;
    SceneNode *m_leftArm;
    SceneNode *m_rightArm;
    SceneNode *m_leftLeg;
    SceneNode *m_rightLeg;

    // Mesh objects for body parts
    MeshObject *m_torsoMesh;
    MeshObject *m_headMesh;
    MeshObject *m_leftArmMesh;
    MeshObject *m_rightArmMesh;
    MeshObject *m_leftLegMesh;
    MeshObject *m_rightLegMesh;

    // Animation state
    std::map<std::string, EntityPose> m_poses;
    std::map<std::string, AnimationSequence> m_sequences;
    AnimationSequence *m_currentSequence = nullptr;
    int m_currentPoseIndex = 0;
    float m_timeInCurrentPose = 0.0f;
    EntityPose *m_sourcePose = nullptr;
    EntityPose *m_targetPose = nullptr;
    EntityState m_currentState = IDLE;

    // Animation methods
    void interpolateBetweenPoses(const EntityPose &pose1, const EntityPose &pose2, float factor);
    void updatePoseAnimation(float deltaTime);

    // Helper for texture coordinates
    static glm::vec4 pixelCoordsToUV(int imageWidth, int imageHeight,
                                     int startX, int startY,
                                     int endX, int endY,
                                     bool flipY = true, bool flipX = true);
};