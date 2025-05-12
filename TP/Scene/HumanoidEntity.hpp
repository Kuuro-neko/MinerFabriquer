#pragma once
#include "Entity.hpp"
#include <glm/gtx/quaternion.hpp>

// Forward declarations
class HumanoidEntity;

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

enum EntityState
{
    IDLE,
    WALKING,
    ATTACKING
};

/**
 * Base class for all humanoid entities (with head, arms, legs)
 */
class HumanoidEntity : public Entity
{
public:
    // Animation state
    std::map<std::string, EntityPose> m_poses;

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
    void setTexture(PBRTexture *texture) override;
    void draw(GLuint programID) override;
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
    VoxelMeshObject *m_torsoMesh;
    VoxelMeshObject *m_headMesh;
    VoxelMeshObject *m_leftArmMesh;
    VoxelMeshObject *m_rightArmMesh;
    VoxelMeshObject *m_leftLegMesh;
    VoxelMeshObject *m_rightLegMesh;

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

// Function declaration - move implementation to cpp file
void create_cube_textured2(glm::vec3 size, VoxelMeshObject &mesh,
                           glm::vec4 uvNorth, glm::vec4 uvSouth,
                           glm::vec4 uvWest, glm::vec4 uvEast,
                           glm::vec4 uvUp, glm::vec4 uvDown);