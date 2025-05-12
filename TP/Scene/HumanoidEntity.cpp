#include "HumanoidEntity.hpp"

// Helper function for textured cube creation
void create_cube_textured2(glm::vec3 size, MeshObject &mesh,
                           glm::vec4 uvNorth, glm::vec4 uvSouth, glm::vec4 uvWest, glm::vec4 uvEast,
                           glm::vec4 uvUp, glm::vec4 uvDown)
{
    mesh.vertices.clear();
    mesh.triangles.clear();
    mesh.uvs.clear();
    mesh.normals.clear();

    glm::vec3 p[] = {
        {-size.x, -size.y, -size.z},
        {size.x, -size.y, -size.z},
        {size.x, size.y, -size.z},
        {-size.x, size.y, -size.z},
        {-size.x, -size.y, size.z},
        {size.x, -size.y, size.z},
        {size.x, size.y, size.z},
        {-size.x, size.y, size.z}};

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
        uvSouth, // South
        uvNorth, // North
        uvEast,  // East
        uvWest,  // West
        uvUp,    // Up
        uvDown   // Down
    };

    for (int i = 0; i < 6; ++i)
    {
        // 4 vertices for each face
        mesh.vertices.push_back(p[face_indices[i][0]]);
        mesh.vertices.push_back(p[face_indices[i][1]]);
        mesh.vertices.push_back(p[face_indices[i][2]]);
        mesh.vertices.push_back(p[face_indices[i][3]]);

        // UVs corresponding to each face
        // Format of uvs[i]: (uStart, vStart, uEnd, vEnd)
        mesh.uvs.push_back({uvs[i].x, uvs[i].y}); // Bottom-left
        mesh.uvs.push_back({uvs[i].z, uvs[i].y}); // Bottom-right
        mesh.uvs.push_back({uvs[i].z, uvs[i].w}); // Top-right
        mesh.uvs.push_back({uvs[i].x, uvs[i].w}); // Top-left

        // 2 triangles to form the face
        int start = i * 4;
        mesh.triangles.push_back(start);
        mesh.triangles.push_back(start + 1);
        mesh.triangles.push_back(start + 2);

        mesh.triangles.push_back(start);
        mesh.triangles.push_back(start + 2);
        mesh.triangles.push_back(start + 3);
    }

    // Normals for each face
    for (int i = 0; i < 4; ++i)
        mesh.normals.push_back(glm::vec3(0, 0, -1)); // South
    for (int i = 0; i < 4; ++i)
        mesh.normals.push_back(glm::vec3(0, 0, 1)); // North
    for (int i = 0; i < 4; ++i)
        mesh.normals.push_back(glm::vec3(-1, 0, 0)); // West
    for (int i = 0; i < 4; ++i)
        mesh.normals.push_back(glm::vec3(1, 0, 0)); // East
    for (int i = 0; i < 4; ++i)
        mesh.normals.push_back(glm::vec3(0, 1, 0)); // Up
    for (int i = 0; i < 4; ++i)
        mesh.normals.push_back(glm::vec3(0, -1, 0)); // Down
}

void EntityPose::initFromEntity(HumanoidEntity *entity)
{
    headTransform = entity->getHead()->m_transform;
    torsoTransform = entity->m_transform;
    leftArmTransform = entity->getLeftArm()->m_transform;
    rightArmTransform = entity->getRightArm()->m_transform;
    leftLegTransform = entity->getLeftLeg()->m_transform;
    rightLegTransform = entity->getRightLeg()->m_transform;
}

HumanoidEntity::HumanoidEntity() : Entity()
{
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

HumanoidEntity::~HumanoidEntity()
{
    // Base destructor
}

void HumanoidEntity::cleanupBuffers()
{
    // Clean up mesh buffers
    if (m_torsoMesh)
        m_torsoMesh->cleanupBuffers();
    if (m_headMesh)
        m_headMesh->cleanupBuffers();
    if (m_leftArmMesh)
        m_leftArmMesh->cleanupBuffers();
    if (m_rightArmMesh)
        m_rightArmMesh->cleanupBuffers();
    if (m_leftLegMesh)
        m_leftLegMesh->cleanupBuffers();
    if (m_rightLegMesh)
        m_rightLegMesh->cleanupBuffers();

    // Call parent class implementation
    Entity::cleanupBuffers();
}

void HumanoidEntity::draw(GLuint programID)
{
    Entity::draw(programID);
}

void HumanoidEntity::setHeadMesh(MeshObject *mesh)
{
    m_head->m_mesh = mesh;
}

void HumanoidEntity::setTorsoMesh(MeshObject *mesh)
{
    this->m_mesh = mesh;
}

void HumanoidEntity::setLeftArmMesh(MeshObject *mesh)
{
    m_leftArm->m_mesh = mesh;
}

void HumanoidEntity::setRightArmMesh(MeshObject *mesh)
{
    m_rightArm->m_mesh = mesh;
}

void HumanoidEntity::setLeftLegMesh(MeshObject *mesh)
{
    m_leftLeg->m_mesh = mesh;
}

void HumanoidEntity::setRightLegMesh(MeshObject *mesh)
{
    m_rightLeg->m_mesh = mesh;
}

void HumanoidEntity::setTexture(Texture *texture)
{
    Entity::setTexture(texture);
    m_head->m_texture = texture;
    m_leftArm->m_texture = texture;
    m_rightArm->m_texture = texture;
    m_leftLeg->m_texture = texture;
    m_rightLeg->m_texture = texture;
}

glm::vec4 HumanoidEntity::pixelCoordsToUV(int imageWidth, int imageHeight, int startX, int startY, int endX, int endY, bool flipY, bool flipX)
{
    float uStart, uEnd, vStart, vEnd;

    endX += 1;
    endY += 1;

    if (flipX)
    {
        uStart = static_cast<float>(endX) / static_cast<float>(imageWidth);
        uEnd = static_cast<float>(startX) / static_cast<float>(imageWidth);
    }
    else
    {
        uStart = static_cast<float>(startX) / static_cast<float>(imageWidth);
        uEnd = static_cast<float>(endX) / static_cast<float>(imageWidth);
    }

    if (flipY)
    {
        vStart = static_cast<float>(endY) / static_cast<float>(imageHeight);
        vEnd = static_cast<float>(startY) / static_cast<float>(imageHeight);
    }
    else
    {
        vStart = static_cast<float>(startY) / static_cast<float>(imageHeight);
        vEnd = static_cast<float>(endY) / static_cast<float>(imageHeight);
    }

    return glm::vec4(uStart, vStart, uEnd, vEnd);
}

void HumanoidEntity::generateHumanoidMesh(float groundHeight)
{
    float membre_Y = 0.703125f;
    float membre_Z = 0.234375f;
    float membre_X = 0.234375f;

    float torse_Y = 0.703125f;
    float torse_Z = 0.234375f;
    float torse_X = 0.46875f;

    float head = 0.46875f;

    // Head UVs
    glm::vec4 headNorth = pixelCoordsToUV(64, 32, 8, 8, 15, 15);  // Face
    glm::vec4 headSouth = pixelCoordsToUV(64, 32, 24, 8, 31, 15); // South
    glm::vec4 headWest = pixelCoordsToUV(64, 32, 16, 8, 23, 15);  // West
    glm::vec4 headEast = pixelCoordsToUV(64, 32, 0, 8, 7, 15);    // East
    glm::vec4 headUp = pixelCoordsToUV(64, 32, 8, 0, 15, 7);      // Up
    glm::vec4 headDown = pixelCoordsToUV(64, 32, 16, 0, 23, 7);   // Down

    // Torso UVs
    glm::vec4 torsoNorth = pixelCoordsToUV(64, 32, 20, 20, 27, 31); // North
    glm::vec4 torsoSouth = pixelCoordsToUV(64, 32, 32, 20, 39, 31); // South
    glm::vec4 torsoWest = pixelCoordsToUV(64, 32, 16, 20, 19, 31);  // West
    glm::vec4 torsoEast = pixelCoordsToUV(64, 32, 28, 20, 31, 31);  // East
    glm::vec4 torsoUp = pixelCoordsToUV(64, 32, 20, 16, 27, 19);    // Up
    glm::vec4 torsoDown = pixelCoordsToUV(64, 32, 28, 16, 35, 19);  // Down

    // Left Arm UVs
    glm::vec4 leftArmNorth = pixelCoordsToUV(64, 32, 44, 20, 47, 31, true, false); // North
    glm::vec4 leftArmSouth = pixelCoordsToUV(64, 32, 52, 20, 55, 31, true, false); // South
    glm::vec4 leftArmWest = pixelCoordsToUV(64, 32, 40, 20, 43, 31, true, false);  // West
    glm::vec4 leftArmEast = pixelCoordsToUV(64, 32, 48, 20, 51, 31, true, false);  // East
    glm::vec4 leftArmUp = pixelCoordsToUV(64, 32, 44, 16, 47, 19, false, true);    // Up
    glm::vec4 leftArmDown = pixelCoordsToUV(64, 32, 48, 16, 51, 19);               // Down

    // Right Arm UVs
    glm::vec4 rightArmNorth = pixelCoordsToUV(64, 32, 44, 20, 47, 31);             // North
    glm::vec4 rightArmSouth = pixelCoordsToUV(64, 32, 52, 20, 55, 31);             // South
    glm::vec4 rightArmWest = pixelCoordsToUV(64, 32, 48, 20, 51, 31);              // West
    glm::vec4 rightArmEast = pixelCoordsToUV(64, 32, 40, 20, 43, 31);              // East
    glm::vec4 rightArmUp = pixelCoordsToUV(64, 32, 44, 16, 47, 19, true, true);    // Up
    glm::vec4 rightArmDown = pixelCoordsToUV(64, 32, 48, 16, 51, 19, true, false); // Down

    // Left Leg UVs
    glm::vec4 leftLegNorth = pixelCoordsToUV(64, 32, 4, 20, 7, 31);                // North
    glm::vec4 leftLegSouth = pixelCoordsToUV(64, 32, 12, 20, 15, 31, true, false); // South
    glm::vec4 leftLegWest = pixelCoordsToUV(64, 32, 0, 20, 3, 31, true, false);    // West
    glm::vec4 leftLegEast = pixelCoordsToUV(64, 32, 8, 20, 11, 31, true, false);   // East
    glm::vec4 leftLegUp = pixelCoordsToUV(64, 32, 4, 16, 7, 19);                   // Up
    glm::vec4 leftLegDown = pixelCoordsToUV(64, 32, 8, 16, 11, 19);                // Down

    // Right Leg UVs
    glm::vec4 rightLegNorth = pixelCoordsToUV(64, 32, 4, 20, 7, 31);   // North
    glm::vec4 rightLegSouth = pixelCoordsToUV(64, 32, 12, 20, 15, 31); // South
    glm::vec4 rightLegWest = pixelCoordsToUV(64, 32, 8, 20, 11, 31);   // West
    glm::vec4 rightLegEast = pixelCoordsToUV(64, 32, 0, 20, 3, 31);    // East
    glm::vec4 rightLegUp = pixelCoordsToUV(64, 32, 4, 16, 7, 19);      // Up
    glm::vec4 rightLegDown = pixelCoordsToUV(64, 32, 8, 16, 11, 19);   // Down

    // Create torso
    create_cube_textured2(glm::vec3(torse_X, torse_Y, torse_Z) * 0.5f, *m_torsoMesh,
                          torsoNorth, torsoSouth, torsoWest, torsoEast, torsoUp, torsoDown);
    m_torsoMesh->initializeBuffers();
    this->m_mesh = m_torsoMesh;
    this->translate(glm::vec3(0.f, torse_Y - 0.15f, 0.f));

    // Create legs
    create_cube_textured2(glm::vec3(membre_X, membre_Y, membre_Z) * 0.5f, *m_leftLegMesh,
                          leftLegNorth, leftLegSouth, leftLegWest, leftLegEast, leftLegUp, leftLegDown);
    m_leftLegMesh->initializeBuffers();
    m_leftLeg->m_mesh = m_leftLegMesh;

    create_cube_textured2(glm::vec3(membre_X, membre_Y, membre_Z) * 0.5f, *m_rightLegMesh,
                          rightLegNorth, rightLegSouth, rightLegWest, rightLegEast, rightLegUp, rightLegDown);
    m_rightLegMesh->initializeBuffers();
    m_rightLeg->m_mesh = m_rightLegMesh;

    // Position legs
    m_leftLeg->translate(glm::vec3(membre_X * 0.5f, -membre_Y, 0.f));
    m_rightLeg->translate(glm::vec3(-membre_X * 0.5f, -membre_Y, 0.f));

    // Create arms
    create_cube_textured2(glm::vec3(membre_X, membre_Y, membre_Z) * 0.5f, *m_leftArmMesh,
                          leftArmNorth, leftArmSouth, leftArmWest, leftArmEast, leftArmUp, leftArmDown);
    m_leftArmMesh->initializeBuffers();
    m_leftArm->m_mesh = m_leftArmMesh;

    create_cube_textured2(glm::vec3(membre_X, membre_Y, membre_Z) * 0.5f, *m_rightArmMesh,
                          rightArmNorth, rightArmSouth, rightArmWest, rightArmEast, rightArmUp, rightArmDown);
    m_rightArmMesh->initializeBuffers();
    m_rightArm->m_mesh = m_rightArmMesh;

    // Position arms relative to torso
    m_leftArm->translate(glm::vec3(membre_X * 1.5f, 0.f, 0.f));
    m_rightArm->translate(glm::vec3(-membre_X * 1.5f, 0.f, 0.f));

    // Create head and position relative to torso
    create_cube_textured2(glm::vec3(head, head, head) * 0.5f, *m_headMesh,
                          headNorth, headSouth, headWest, headEast, headUp, headDown);
    m_headMesh->initializeBuffers();
    m_head->m_mesh = m_headMesh;
    m_head->translate(glm::vec3(0.f, head * 1.25f, 0.f));

    // TP body to position we want
    translate(glm::vec3(0.f, groundHeight, 0.f));
}

// Animation system implementation

void HumanoidEntity::setState(EntityState newState)
{
    if (newState != m_currentState)
    {
        m_currentState = newState;
        m_timeInCurrentPose = 0.0f;
        m_currentPoseIndex = 0;

        switch (newState)
        {
        case IDLE:
            setCurrentSequence("idle");
            break;
        case WALKING:
            setCurrentSequence("walking");
            break;
        case ATTACKING:
            setCurrentSequence("attacking");
            break;
        default:
            break;
        }
    }
}

void HumanoidEntity::initializeBasePose()
{
    EntityPose basePose("base");
    basePose.initFromEntity(this);
    m_poses["base"] = basePose;
}

void HumanoidEntity::addPose(const std::string &name, const EntityPose &pose)
{
    m_poses[name] = pose;
}

void HumanoidEntity::createAnimationSequence(const std::string &name, const std::vector<std::string> &poseNames,
                                             const std::vector<float> &durations, bool loop)
{
    if (poseNames.size() != durations.size())
    {
        std::cerr << "Error: Number of poses and durations must match" << std::endl;
        return;
    }

    AnimationSequence sequence;
    sequence.loop = loop;

    for (size_t i = 0; i < poseNames.size(); i++)
    {
        if (m_poses.find(poseNames[i]) != m_poses.end())
        {
            sequence.keyPoses.push_back(m_poses[poseNames[i]]);
            sequence.durations.push_back(durations[i]);
        }
        else
        {
            std::cerr << "Warning: Pose '" << poseNames[i] << "' not found" << std::endl;
        }
    }

    m_sequences[name] = sequence;
}

void HumanoidEntity::setCurrentSequence(const std::string &sequenceName)
{
    if (m_sequences.find(sequenceName) != m_sequences.end())
    {
        m_currentSequence = &m_sequences[sequenceName];
        m_currentPoseIndex = 0;
        m_timeInCurrentPose = 0.0f;

        if (!m_currentSequence->keyPoses.empty())
        {
            m_sourcePose = &m_currentSequence->keyPoses[0];
            int targetIndex = (m_currentPoseIndex + 1) % m_currentSequence->keyPoses.size();
            m_targetPose = &m_currentSequence->keyPoses[targetIndex];
        }
    }
    else
    {
        std::cerr << "Warning: Animation sequence '" << sequenceName << "' not found" << std::endl;
    }
}

void HumanoidEntity::updateAnimation(float deltaTime)
{
    updatePoseAnimation(deltaTime);
}

void HumanoidEntity::updatePoseAnimation(float deltaTime)
{
    if (!m_currentSequence || m_currentSequence->keyPoses.empty())
    {
        return;
    }

    m_timeInCurrentPose += deltaTime;

    int targetIndex = (m_currentPoseIndex + 1) % m_currentSequence->keyPoses.size();

    if (m_timeInCurrentPose >= m_currentSequence->durations[m_currentPoseIndex])
    {
        m_timeInCurrentPose = 0.0f;
        m_currentPoseIndex = targetIndex;

        if (m_currentPoseIndex == 0 && !m_currentSequence->loop)
        {
            // If the sequence should not loop, return to idle animation
            setState(IDLE);
            return;
        }

        m_sourcePose = &m_currentSequence->keyPoses[m_currentPoseIndex];
        targetIndex = (m_currentPoseIndex + 1) % m_currentSequence->keyPoses.size();
        m_targetPose = &m_currentSequence->keyPoses[targetIndex];
    }

    float factor = m_timeInCurrentPose / m_currentSequence->durations[m_currentPoseIndex];

    interpolateBetweenPoses(*m_sourcePose, *m_targetPose, factor);
}

void HumanoidEntity::interpolateBetweenPoses(const EntityPose &pose1, const EntityPose &pose2, float factor)
{
    factor = glm::clamp(factor, 0.0f, 1.0f);

    // Interpolate rotations with quaternions
    // Head
    glm::quat sourceRot = glm::quat_cast(glm::mat4(pose1.headTransform.m_rotation));
    glm::quat targetRot = glm::quat_cast(glm::mat4(pose2.headTransform.m_rotation));
    glm::quat interpRot = glm::slerp(sourceRot, targetRot, factor);
    m_head->m_transform.m_rotation = glm::mat3x3(glm::mat4_cast(interpRot));

    // Head translation
    glm::vec3 sourcePos = pose1.headTransform.m_translation;
    glm::vec3 targetPos = pose2.headTransform.m_translation;
    m_head->m_transform.m_translation = glm::mix(sourcePos, targetPos, factor);

    // Left arm
    sourceRot = glm::quat_cast(glm::mat4(pose1.leftArmTransform.m_rotation));
    targetRot = glm::quat_cast(glm::mat4(pose2.leftArmTransform.m_rotation));
    interpRot = glm::slerp(sourceRot, targetRot, factor);
    m_leftArm->m_transform.m_rotation = glm::mat3x3(glm::mat4_cast(interpRot));

    sourcePos = pose1.leftArmTransform.m_translation;
    targetPos = pose2.leftArmTransform.m_translation;
    m_leftArm->m_transform.m_translation = glm::mix(sourcePos, targetPos, factor);

    // Right arm
    sourceRot = glm::quat_cast(glm::mat4(pose1.rightArmTransform.m_rotation));
    targetRot = glm::quat_cast(glm::mat4(pose2.rightArmTransform.m_rotation));
    interpRot = glm::slerp(sourceRot, targetRot, factor);
    m_rightArm->m_transform.m_rotation = glm::mat3x3(glm::mat4_cast(interpRot));

    sourcePos = pose1.rightArmTransform.m_translation;
    targetPos = pose2.rightArmTransform.m_translation;
    m_rightArm->m_transform.m_translation = glm::mix(sourcePos, targetPos, factor);

    // Left leg
    sourceRot = glm::quat_cast(glm::mat4(pose1.leftLegTransform.m_rotation));
    targetRot = glm::quat_cast(glm::mat4(pose2.leftLegTransform.m_rotation));
    interpRot = glm::slerp(sourceRot, targetRot, factor);
    m_leftLeg->m_transform.m_rotation = glm::mat3x3(glm::mat4_cast(interpRot));

    sourcePos = pose1.leftLegTransform.m_translation;
    targetPos = pose2.leftLegTransform.m_translation;
    m_leftLeg->m_transform.m_translation = glm::mix(sourcePos, targetPos, factor);

    // Right leg
    sourceRot = glm::quat_cast(glm::mat4(pose1.rightLegTransform.m_rotation));
    targetRot = glm::quat_cast(glm::mat4(pose2.rightLegTransform.m_rotation));
    interpRot = glm::slerp(sourceRot, targetRot, factor);
    m_rightLeg->m_transform.m_rotation = glm::mat3x3(glm::mat4_cast(interpRot));

    sourcePos = pose1.rightLegTransform.m_translation;
    targetPos = pose2.rightLegTransform.m_translation;
    m_rightLeg->m_transform.m_translation = glm::mix(sourcePos, targetPos, factor);

    // Update model matrices after interpolation
    m_head->updateModelMatrix();
    m_leftArm->updateModelMatrix();
    m_rightArm->updateModelMatrix();
    m_leftLeg->updateModelMatrix();
    m_rightLeg->updateModelMatrix();
}