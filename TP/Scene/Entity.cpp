#include "Entity.hpp"
void create_cube_textured2(glm::vec3 size, MeshObject &mesh) {
    mesh.vertices.clear();
    mesh.triangles.clear();
    mesh.uvs.clear();

    glm::vec3 p[] = {
            {-size.x, -size.y, -size.z},
            { size.x, -size.y, -size.z},
            { size.x,  size.y, -size.z},
            {-size.x,  size.y, -size.z},
            {-size.x, -size.y,  size.z},
            { size.x, -size.y,  size.z},
            { size.x,  size.y,  size.z},
            {-size.x,  size.y,  size.z}
    };

    // Définir les faces du cube avec 4 sommets par face
    int face_indices[6][4] = {
            {0, 1, 2, 3}, // back
            {5, 4, 7, 6}, // front
            {4, 0, 3, 7}, // left
            {1, 5, 6, 2}, // right
            {3, 2, 6, 7}, // top
            {4, 5, 1, 0}  // bottom
    };

    for (int i = 0; i < 6; ++i) {
        // 4 sommets pour chaque face
        mesh.vertices.push_back(p[face_indices[i][0]]);
        mesh.vertices.push_back(p[face_indices[i][1]]);
        mesh.vertices.push_back(p[face_indices[i][2]]);
        mesh.vertices.push_back(p[face_indices[i][3]]);

        // UVs correspondants (même pour chaque face)
        mesh.uvs.push_back({0.0f, 0.0f});
        mesh.uvs.push_back({1.0f, 0.0f});
        mesh.uvs.push_back({1.0f, 1.0f});
        mesh.uvs.push_back({0.0f, 1.0f});

        // 2 triangles pour former la face
        int start = i * 4;
        mesh.triangles.push_back(start);
        mesh.triangles.push_back(start + 1);
        mesh.triangles.push_back(start + 2);

        mesh.triangles.push_back(start);
        mesh.triangles.push_back(start + 2);
        mesh.triangles.push_back(start + 3);
    }
}

Entity::Entity() {
    m_head = new SceneNode();
    m_leftArm = new SceneNode();
    m_rightArm = new SceneNode();
    m_leftLeg = new SceneNode();
    m_rightLeg = new SceneNode();

    m_torsoMesh = new MeshObject();
    m_headMesh = new MeshObject();
    m_leftArmMesh = new MeshObject();
    m_rightArmMesh = new MeshObject();
    m_leftLegMesh = new MeshObject();
    m_rightLegMesh = new MeshObject();

    
    this->addChild(m_head);
    this->addChild(m_leftArm);
    this->addChild(m_rightArm);
    this->addChild(m_leftLeg);
    this->addChild(m_rightLeg);
}

void Entity::cleanupBuffers() {
    // Clean up mesh buffers
    if (m_torsoMesh) m_torsoMesh->cleanupBuffers();
    if (m_headMesh) m_headMesh->cleanupBuffers();
    if (m_leftArmMesh) m_leftArmMesh->cleanupBuffers();
    if (m_rightArmMesh) m_rightArmMesh->cleanupBuffers();
    if (m_leftLegMesh) m_leftLegMesh->cleanupBuffers();
    if (m_rightLegMesh) m_rightLegMesh->cleanupBuffers();
    
    // Call parent class implementation
    SceneNode::cleanupBuffers();
}

Entity::~Entity() {
    // heritage SceneNode destructor
}

void Entity::draw(GLuint programID) {
    SceneNode::draw(programID);
}

void Entity::setHeadMesh(MeshObject* mesh) {
    m_head->m_mesh = mesh;
}

void Entity::setTorsoMesh(MeshObject* mesh) {
    this->m_mesh = mesh;
}

void Entity::setLeftArmMesh(MeshObject* mesh) {
    m_leftArm->m_mesh = mesh;
}

void Entity::setRightArmMesh(MeshObject* mesh) {
    m_rightArm->m_mesh = mesh;
}

void Entity::setLeftLegMesh(MeshObject* mesh) {
    m_leftLeg->m_mesh = mesh;
}

void Entity::setRightLegMesh(MeshObject* mesh) {
    m_rightLeg->m_mesh = mesh;
}

void Entity::generateHumanoidMesh(float baseHeight) {
    float membre_Y = 0.703125f;
    float membre_Z = 0.234375f;
    float membre_X = 0.234375f;
    
    float torse_Y = 0.703125f;
    float torse_Z = 0.234375f;
    float torse_X = 0.46875f;
    
    float head = 0.46875f;



    //create torso
    create_cube_textured2(glm::vec3(torse_X, torse_Y, torse_Z)*0.5f, *m_torsoMesh);
    m_torsoMesh->initializeBuffers();
    this->m_mesh = m_torsoMesh;
    this->translate(glm::vec3(0.f, torse_Y-0.15f, 0.f));
    
    //create legs
    create_cube_textured2(glm::vec3(membre_X, membre_Y, membre_Z)*0.5f, *m_leftLegMesh);
    m_leftLegMesh->initializeBuffers();
    m_leftLeg->m_mesh = m_leftLegMesh;
    
    create_cube_textured2(glm::vec3(membre_X, membre_Y, membre_Z)*0.5f, *m_rightLegMesh);
    m_rightLegMesh->initializeBuffers();
    m_rightLeg->m_mesh = m_rightLegMesh;
    
    //position legs
    m_leftLeg->translate(glm::vec3(-membre_X*0.5f, -membre_Y, 0.f));
    m_rightLeg->translate(glm::vec3(membre_X*0.5f, -membre_Y, 0.f));

    
    
    //create arms
    create_cube_textured2(glm::vec3(membre_X, membre_Y, membre_Z)*0.5f, *m_leftArmMesh);
    m_leftArmMesh->initializeBuffers();
    m_leftArm->m_mesh = m_leftArmMesh;
    
    create_cube_textured2(glm::vec3(membre_X, membre_Y, membre_Z)*0.5f, *m_rightArmMesh);
    m_rightArmMesh->initializeBuffers();
    m_rightArm->m_mesh = m_rightArmMesh;
    
    //position arms relative to torso
    m_leftArm->translate(glm::vec3(-membre_X*1.5f, 0.f, 0.f));
    m_rightArm->translate(glm::vec3(membre_X*1.5f, 0.f, 0.f));
    
    //create head and position relative to torso
    create_cube_textured2(glm::vec3(head, head, head)*0.5f, *m_headMesh);
    m_headMesh->initializeBuffers();
    m_head->m_mesh = m_headMesh;
    m_head->translate(glm::vec3(0.f, head*1.2f, 0.f));
    
    //TP body to position we want
    translate(glm::vec3(0.f, baseHeight, 5.f));
}