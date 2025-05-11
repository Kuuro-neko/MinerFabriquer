#pragma once
#include <TP/Scene/SceneNode.hpp>
#include <TP/Scene/MeshObject.hpp>
#include <TP/Scene/Texture.hpp>
#include <utils/Transform.hpp>
#include <map>
#include <string>
#include <vector>
#include <glm/gtx/quaternion.hpp>
class Entity;

struct EntityPose {
    std::string name;
    
    Transform headTransform;
    Transform torsoTransform; 
    Transform leftArmTransform;
    Transform rightArmTransform;
    Transform leftLegTransform;
    Transform rightLegTransform;
    
    EntityPose(std::string poseName = "") : name(poseName) {}
    
    void initFromEntity(Entity* entity);
};

struct AnimationSequence {
    std::vector<EntityPose> keyPoses;
    std::vector<float> durations;
    bool loop;
    
    AnimationSequence() : loop(true) {}
};

enum EntityState {
    IDLE,
    WALKING,
    ATTACKING
};

class Entity : public SceneNode {
    public:
    Entity();

    
    virtual ~Entity();
    
    SceneNode* getHead() { return m_head; }
    SceneNode* getLeftArm() { return m_leftArm; }
    SceneNode* getRightArm() { return m_rightArm; }
    SceneNode* getLeftLeg() { return m_leftLeg; }
    SceneNode* getRightLeg() { return m_rightLeg; }
    
    void setHeadMesh(VoxelMeshObject* mesh);
    void setTorsoMesh(VoxelMeshObject* mesh);
    void setLeftArmMesh(VoxelMeshObject* mesh);
    void setRightArmMesh(VoxelMeshObject* mesh);
    void setLeftLegMesh(VoxelMeshObject* mesh);
    void setRightLegMesh(VoxelMeshObject* mesh);
    void setTexture(PBRTexture* texture);
    virtual void draw(GLuint programID) override;
    void cleanupBuffers() override;

    PBRTexture* m_pbr_texture = nullptr;

    inline void setIsFPS(bool* FPSActive) { this->FPSActive = FPSActive; }


    //you are already in the the torso
    SceneNode* m_head;
    SceneNode* m_leftArm;
    SceneNode* m_rightArm;
    SceneNode* m_leftLeg;
    SceneNode* m_rightLeg;

    VoxelMeshObject *m_torsoMesh;
    VoxelMeshObject *m_headMesh;
    VoxelMeshObject *m_leftArmMesh;
    VoxelMeshObject *m_rightArmMesh;
    VoxelMeshObject *m_leftLegMesh;
    VoxelMeshObject *m_rightLegMesh;

    bool *FPSActive = nullptr;
    void generateHumanoidMesh(float ground);

    void setFPSActive(bool *attached);
    bool isFPSActive();



    std::map<std::string, EntityPose> m_poses;
    std::map<std::string, AnimationSequence> m_sequences;
    AnimationSequence* m_currentSequence = nullptr;
    int m_currentPoseIndex = 0;
    float m_timeInCurrentPose = 0.0f;
    EntityPose* m_sourcePose = nullptr;
    EntityPose* m_targetPose = nullptr;
    EntityState m_currentState = IDLE;
    
    virtual void setState(EntityState newState);
    virtual void updateAnimation(float deltaTime);
    void initializeBasePose();
    void addPose(const std::string& name, const EntityPose& pose);
    void createAnimationSequence(const std::string& name, const std::vector<std::string>& poseNames, 
                                const std::vector<float>& durations, bool loop = true);
    void setCurrentSequence(const std::string& sequenceName);
    virtual void update(float deltaTime);
    void interpolateBetweenPoses(const EntityPose& pose1, const EntityPose& pose2, float factor);
    void updatePoseAnimation(float deltaTime);
};