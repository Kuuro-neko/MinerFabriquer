#include <TP/Entities/Projectiles/TNTProjectile.hpp>
#include <TP/Scene/BlocTypes.hpp>
#include <TP/Entities/HumanoidEntity.hpp>
#include <utils/GLUtils.hpp>
#include "TNTProjectile.hpp"


TNTProjectile::TNTProjectile(glm::vec3 position, glm::vec3 velocity, float radius, World *world, GLuint programID) : Entity()
{
    m_programID = programID;
    m_world = world;
    m_velocity = velocity * speed;
    m_radius = radius;
    generateTNTMesh();
    SceneNode::translate(position);
}

TNTProjectile::~TNTProjectile()
{
    clear();
    m_tntMesh->cleanupBuffers();
}

void TNTProjectile::explode(int x, int y, int z)
{;
    m_world->setBloc(x, y, z, AIR);

    // Create an explosion effect
    for (int i = -m_explosionRadius; i <= m_explosionRadius; ++i)
    {
        for (int j = -m_explosionRadius; j <= m_explosionRadius; ++j)
        {
            for (int k = -m_explosionRadius; k <= m_explosionRadius; ++k)
            {
                float dist = std::sqrt(i * i + j * j + k * k);
                if (dist > m_explosionRadius)
                    continue;
                m_world->removeBlock(x + i, y + j, z + k);
            }
        }
    }

    m_world->removeTNT(this);
}

void TNTProjectile::generateTNTMesh()
{
    BlockData* tntData = BlocDatabase::getInstance().getBloc(TNT);
    m_tntMesh = std::make_shared<VoxelMeshObject>();

    m_tntMesh->vertices.clear();
    m_tntMesh->triangles.clear();
    m_tntMesh->uvs.clear();
    m_tntMesh->normals.clear();
    m_tntMesh->lights.clear();
    m_tntMesh->ao.clear();

    addSquareGeometry(m_tntMesh, TNT, FACE_BOTTOM, 0.f, 0.f, 0.f, false, m_radius);
    addSquareGeometry(m_tntMesh, TNT, FACE_TOP, 0.f, 0.f, 0.f, false, m_radius);
    addSquareGeometry(m_tntMesh, TNT, FACE_WEST, 0.f, 0.f, 0.f, false, m_radius);
    addSquareGeometry(m_tntMesh, TNT, FACE_EAST, 0.f, 0.f, 0.f, false, m_radius);
    addSquareGeometry(m_tntMesh, TNT, FACE_NORTH, 0.f, 0.f, 0.f, false, m_radius);
    addSquareGeometry(m_tntMesh, TNT, FACE_SOUTH, 0.f, 0.f, 0.f, false, m_radius);

    for (int i = 0; i < m_tntMesh->vertices.size(); i++)
    {
        m_tntMesh->lights.push_back(15);
        m_tntMesh->ao.push_back(3);
    }

    m_tntMesh->initializeBuffers();
}

void TNTProjectile::draw(GLuint programID)
{
    PBRTextureAtlas::getInstance().bind(programID);
    GLuint modelMatrixId = glGetUniformLocation(programID, "ModelMatrix");
    glUniformMatrix4fv(modelMatrixId, 1, false, &ModelMatrix[0][0]);
    m_tntMesh->draw(programID);
}

void TNTProjectile::update(float deltaTime)
{
    translate(m_velocity * deltaTime);
    updateBoundingBox();

    // Check for collisions with blocks
    glm::vec3 minBB = getMinBoundingBox();
    glm::vec3 maxBB = getMaxBoundingBox();

    for (int x = static_cast<int>(std::floor(minBB.x)); x <= static_cast<int>(std::floor(maxBB.x)); ++x)
    {
        for (int y = static_cast<int>(std::floor(minBB.y)); y <= static_cast<int>(std::floor(maxBB.y)); ++y)
        {
            for (int z = static_cast<int>(std::floor(minBB.z)); z <= static_cast<int>(std::floor(maxBB.z)); ++z)
            {
                int blockId = m_world->getBloc(x, y, z);

                if (blockId != AIR)
                {
                    explode(x, y, z);
                    return;
                }
            }
        }
    }
}

void TNTProjectile::updateBoundingBox()
{
    AABBmin = getWorldPosition();
    AABBmax = getWorldPosition() + glm::vec3(m_radius, m_radius, m_radius);
}

glm::vec3 TNTProjectile::getMinBoundingBox()
{
    return AABBmin;
}

glm::vec3 TNTProjectile::getMaxBoundingBox()
{
    return AABBmax;
}

void TNTProjectile::clear()
{
    m_tntMesh->cleanupBuffers();
    m_tntMesh->vertices.clear();
    m_tntMesh->triangles.clear();
    m_tntMesh->uvs.clear();
    m_tntMesh->normals.clear();
    m_tntMesh->lights.clear();
    m_tntMesh->ao.clear();
}
