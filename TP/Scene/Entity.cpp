#include "Entity.hpp"
void create_cube_textured2(glm::vec3 size, MeshObject &mesh, 
    glm::vec4 uvNorth = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
    glm::vec4 uvSouth = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
    glm::vec4 uvWest = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
    glm::vec4 uvEast = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
    glm::vec4 uvUp = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
    glm::vec4 uvDown = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)) {
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

    // Define faces with 4 vertices per face
    int face_indices[6][4] = {
        {0, 1, 2, 3}, // South (back, -Z)
        {5, 4, 7, 6}, // North (front, +Z)
        {4, 0, 3, 7}, // West (left, -X)
        {1, 5, 6, 2}, // West (right, +X)
        {3, 2, 6, 7}, // Up (top, +Y)
        {4, 5, 1, 0}  // Down (bottom, -Y)
    };

    // Array of UV coordinates for each face
    glm::vec4 uvs[6] = {
        uvSouth,  // South
        uvNorth,  // North
        uvEast,   // East
        uvWest,   // West
        uvUp,     // Up
        uvDown    // Down
    };

    for (int i = 0; i < 6; ++i) {
        // 4 vertices for each face
        mesh.vertices.push_back(p[face_indices[i][0]]);
        mesh.vertices.push_back(p[face_indices[i][1]]);
        mesh.vertices.push_back(p[face_indices[i][2]]);
        mesh.vertices.push_back(p[face_indices[i][3]]);

        // UVs corresponding to each face
        // Format of uvs[i]: (uStart, vStart, uEnd, vEnd)
        mesh.uvs.push_back({uvs[i].x, uvs[i].y});                // Bottom-left
        mesh.uvs.push_back({uvs[i].z, uvs[i].y});                // Bottom-right
        mesh.uvs.push_back({uvs[i].z, uvs[i].w});                // Top-right
        mesh.uvs.push_back({uvs[i].x, uvs[i].w});                // Top-left

        // 2 triangles to form the face
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

/**
 * Converts start and end pixel coordinates to UV coordinates
 * @param imageWidth Width of the texture image in pixels
 * @param imageHeight Height of the texture image in pixels
 * @param startX X-coordinate of the starting pixel (from left)
 * @param startY Y-coordinate of the starting pixel (from top)
 * @param endX X-coordinate of the ending pixel (from left)
 * @param endY Y-coordinate of the ending pixel (from top)
 * @param flipY Whether to flip the Y-coordinate (default: true, for OpenGL textures)
 * @param flipX Whether to flip the X-coordinate (default: false)
 * @return glm::vec4 containing (uStart, vStart, uEnd, vEnd)
 */
glm::vec4 pixelCoordsToUV(int imageWidth, int imageHeight, int startX, int startY, int endX, int endY, bool flipY = true, bool flipX = true) {
    // Convert pixel coordinates to normalized UV coordinates
    float uStart, uEnd, vStart, vEnd;

    endX +=1;
    endY +=1;

    if(flipX){
        // In OpenGL, U=0 is at the left, but in most image formats, X=0 is at the right
        uStart = static_cast<float>(endX) / static_cast<float>(imageWidth);
        uEnd = static_cast<float>(startX) / static_cast<float>(imageWidth);

        // std::cout <<"uStart : " <<endX << "/" << imageWidth << std::endl;
        // std::cout << "uEnd : " << startX << "/" << imageWidth << std::endl;
    } else {
        uStart = static_cast<float>(startX) / static_cast<float>(imageWidth);
        uEnd = static_cast<float>(endX) / static_cast<float>(imageWidth);
    }



    
    
    if (flipY) {
        // In OpenGL, V=0 is at the bottom, but in most image formats, Y=0 is at the top
        vStart = static_cast<float>(endY) / static_cast<float>(imageHeight);
        vEnd = static_cast<float>(startY) / static_cast<float>(imageHeight);

        // std::cout << "vStart : " << endY << "/" << imageHeight << std::endl;
        // std::cout << "vEnd : " << startY << "/" << imageHeight << std::endl;
    } else {
        vStart = static_cast<float>(startY) / static_cast<float>(imageHeight);
        vEnd = static_cast<float>(endY) / static_cast<float>(imageHeight);
    }




    
    // Return in format expected by create_cube_textured2: (uStart, vStart, uEnd, vEnd)
    return glm::vec4(uStart, vStart, uEnd, vEnd);
}

void Entity::generateHumanoidMesh(float baseHeight) {
    float membre_Y = 0.703125f;
    float membre_Z = 0.234375f;
    float membre_X = 0.234375f;
    
    float torse_Y = 0.703125f;
    float torse_Z = 0.234375f;
    float torse_X = 0.46875f;
    
    float head = 0.46875f;

    /*glm::vec4 headNorth = glm::vec4(0.1f, 0.12f, 0.3f, 0.4f);
    (0,1)                               (1,1)
    +------------------------------------+
    |                                    |
    |     (0.1,0.4)         (0.3, 0.4)   |
    |        +---------------------+     |
    |        |                     |     |
    |        |                     |     |
    |        +---------------------+     |
    |     (0.1,0.12)       (0.3, 0.12)   |
    |                                    |
    +------------------------------------+
    (0,0)                              (1,0)
    
    so
    glm::vec4 surface = glm::vec4(Ustart, Vstart, Uend, Vend);

    pixelCoordsToUV(64,32,)
    
    */

    //Head UVs
    glm::vec4 headNorth = pixelCoordsToUV(64,32,8,8,15,15);   // Face
    glm::vec4 headSouth = pixelCoordsToUV(64,32,24,8,31,15);     // South
    glm::vec4 headWest = pixelCoordsToUV(64,32,16,8,23,15);     // West
    glm::vec4 headEast = pixelCoordsToUV(64,32,0,8,7,15);   // East
    glm::vec4 headUp = pixelCoordsToUV(64,32,8,0,15,7);      // Up
    glm::vec4 headDown = pixelCoordsToUV(64,32,16,0,23,7);   // Down
    
    //Torso UVs
    glm::vec4 torsoNorth = pixelCoordsToUV(64,32,20,20,27,31);  // North
    glm::vec4 torsoSouth = pixelCoordsToUV(64,32,32,20,39,31);    // South
    glm::vec4 torsoWest = pixelCoordsToUV(64,32,16,20,19,31);   // West
    glm::vec4 torsoEast = pixelCoordsToUV(64,32,28,20,31,31);   // East
    glm::vec4 torsoUp = pixelCoordsToUV(64,32,20,16,27,19);    // Up
    glm::vec4 torsoDown = pixelCoordsToUV(64,32,28,16,35,19);  // Down
    
    //Left Arm UVs
    glm::vec4 leftArmNorth = pixelCoordsToUV(64,32,44,20,47,31,true,false);  // North
    glm::vec4 leftArmSouth = pixelCoordsToUV(64,32,52,20,55,31, true,false);   // South
    glm::vec4 leftArmWest = pixelCoordsToUV(64,32,40,20,43,31,true,false);   // West
    glm::vec4 leftArmEast = pixelCoordsToUV(64,32,48,20,51,31,true, false);  // East
    glm::vec4 leftArmUp = pixelCoordsToUV(64,32,44,16,47,19,false,true);    // Up
    glm::vec4 leftArmDown = pixelCoordsToUV(64,32,48,16,51,19); // Down
    
    //Right Arm UVs
    glm::vec4 rightArmNorth = pixelCoordsToUV(64,32,44,20,47,31);  // North
    glm::vec4 rightArmSouth = pixelCoordsToUV(64,32,52,20,55,31);   // South
    glm::vec4 rightArmWest = pixelCoordsToUV(64,32,48,20,51,31);    // West
    glm::vec4 rightArmEast = pixelCoordsToUV(64,32,40,20,43,31); // East
    glm::vec4 rightArmUp = pixelCoordsToUV(64,32,44,16,47,19, true, true);   // Up
    glm::vec4 rightArmDown = pixelCoordsToUV(64,32,48,16,51,19,true,false); // Down
    
    //Left Leg UVs
    glm::vec4 leftLegNorth = pixelCoordsToUV(64,32,4,20,7,31);  // North
    glm::vec4 leftLegSouth = pixelCoordsToUV(64,32,12,20,15,31,true,false);    // South
    glm::vec4 leftLegWest = pixelCoordsToUV(64,32,0,20,3,31,true,false);     // West
    glm::vec4 leftLegEast = pixelCoordsToUV(64,32,8,20,11,31,true,false);  // East
    glm::vec4 leftLegUp = pixelCoordsToUV(64,32,4,16,7,19);   // Up
    glm::vec4 leftLegDown = pixelCoordsToUV(64,32,8,16,11,19); // Down
    
    //Right Leg UVs
    glm::vec4 rightLegNorth = pixelCoordsToUV(64,32,4,20,7,31);  // North
    glm::vec4 rightLegSouth = pixelCoordsToUV(64,32,12,20,15,31);  // South
    glm::vec4 rightLegWest = pixelCoordsToUV(64,32,8,20,11,31);   // West
    glm::vec4 rightLegEast = pixelCoordsToUV(64,32,0,20,3,31); // East
    glm::vec4 rightLegUp = pixelCoordsToUV(64,32,4,16,7,19);   // Up
    glm::vec4 rightLegDown = pixelCoordsToUV(64,32,8,16,11,19); // Down


    //create torso
    create_cube_textured2(glm::vec3(torse_X, torse_Y, torse_Z)*0.5f, *m_torsoMesh,
                        torsoNorth, torsoSouth, torsoWest, torsoEast, torsoUp, torsoDown);
    m_torsoMesh->initializeBuffers();
    this->m_mesh = m_torsoMesh;
    this->translate(glm::vec3(0.f, torse_Y-0.15f, 0.f));
    
    //create legs
    create_cube_textured2(glm::vec3(membre_X, membre_Y, membre_Z)*0.5f, *m_leftLegMesh,
                        leftLegNorth, leftLegSouth, leftLegWest, leftLegEast, leftLegUp, leftLegDown);
    m_leftLegMesh->initializeBuffers();
    m_leftLeg->m_mesh = m_leftLegMesh;
    
    create_cube_textured2(glm::vec3(membre_X, membre_Y, membre_Z)*0.5f, *m_rightLegMesh,
                        rightLegNorth, rightLegSouth, rightLegWest, rightLegEast, rightLegUp, rightLegDown);
    m_rightLegMesh->initializeBuffers();
    m_rightLeg->m_mesh = m_rightLegMesh;
    
    //position legs
    m_leftLeg->translate(glm::vec3(membre_X*0.5f, -membre_Y, 0.f));
    m_rightLeg->translate(glm::vec3(-membre_X*0.5f, -membre_Y, 0.f));

    
    
    //create arms
    create_cube_textured2(glm::vec3(membre_X, membre_Y, membre_Z)*0.5f, *m_leftArmMesh,
                        leftArmNorth, leftArmSouth, leftArmWest, leftArmEast, leftArmUp, leftArmDown);
    m_leftArmMesh->initializeBuffers();
    m_leftArm->m_mesh = m_leftArmMesh;
    
    create_cube_textured2(glm::vec3(membre_X, membre_Y, membre_Z)*0.5f, *m_rightArmMesh,
                        rightArmNorth, rightArmSouth, rightArmWest, rightArmEast, rightArmUp, rightArmDown);
    m_rightArmMesh->initializeBuffers();
    m_rightArm->m_mesh = m_rightArmMesh;
    
    //position arms relative to torso
    m_leftArm->translate(glm::vec3(membre_X*1.5f, 0.f, 0.f));
    m_rightArm->translate(glm::vec3(-membre_X*1.5f, 0.f, 0.f));


    
    //create head and position relative to torso
    create_cube_textured2(glm::vec3(head, head, head)*0.5f, *m_headMesh,
                        headNorth, headSouth, headWest, headEast, headUp, headDown);
    m_headMesh->initializeBuffers();
    m_head->m_mesh = m_headMesh;
    m_head->translate(glm::vec3(0.f, head*1.25f, 0.f));

    
    //TP body to position we want
    translate(glm::vec3(0.f, baseHeight, 5.f));
}


void Entity::setTexture(Texture* texture) {

    this->m_texture = texture;
    m_head->m_texture = texture;
    m_leftArm->m_texture = texture;
    m_rightArm->m_texture = texture;
    m_leftLeg->m_texture = texture;
    m_rightLeg->m_texture = texture;
}