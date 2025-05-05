#pragma once
#include <TP/Scene/SceneNode.hpp>
#include <TP/Scene/MeshObject.hpp>
#include <TP/Scene/Texture.hpp>
#include <utils/Transform.hpp>

class Entity : public SceneNode {
    public:
    Entity();

    
    virtual ~Entity();
    
    SceneNode* getHead() { return m_head; }
    SceneNode* getLeftArm() { return m_leftArm; }
    SceneNode* getRightArm() { return m_rightArm; }
    SceneNode* getLeftLeg() { return m_leftLeg; }
    SceneNode* getRightLeg() { return m_rightLeg; }
    
    void setHeadMesh(MeshObject* mesh);
    void setTorsoMesh(MeshObject* mesh);
    void setLeftArmMesh(MeshObject* mesh);
    void setRightArmMesh(MeshObject* mesh);
    void setLeftLegMesh(MeshObject* mesh);
    void setRightLegMesh(MeshObject* mesh);
    void setTexture(Texture* texture);
    void draw(GLuint programID) override;
    void cleanupBuffers() override;

    inline void setIsFPS(bool* FPSActive) { this->FPSActive = FPSActive; }


    //you are already in the the torso
    SceneNode* m_head;
    SceneNode* m_leftArm;
    SceneNode* m_rightArm;
    SceneNode* m_leftLeg;
    SceneNode* m_rightLeg;

    MeshObject *m_torsoMesh;
    MeshObject *m_headMesh;
    MeshObject *m_leftArmMesh;
    MeshObject *m_rightArmMesh;
    MeshObject *m_leftLegMesh;
    MeshObject *m_rightLegMesh;

    bool *FPSActive = nullptr;
    void generateHumanoidMesh(float ground);

    void setFPSActive(bool *attached);




    bool isFPSActive();
};