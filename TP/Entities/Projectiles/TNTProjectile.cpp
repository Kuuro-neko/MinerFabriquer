#include "TNTProjectile.hpp"
#include <TP/Scene/BlocTypes.hpp>
#include <utils/GLUtils.hpp>

TNTProjectile::TNTProjectile(glm::vec3 position, glm::vec3 velocity, float radius, World *world, GLuint programID)
    : Projectile(position, velocity * TNT_PROJ_SPEED, radius, TNT_PROJ_SPEED, world, programID)
{
    generateTNTMesh();
}

TNTProjectile::~TNTProjectile()
{
    clear();
}

void TNTProjectile::explode(int x, int y, int z)
{
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
                m_world->playerRemoveBlock(x + i, y + j, z + k, GAMEMODE_SURVIVAL);
            }
        }
    }

    m_world->removeTNT(this);
}

void TNTProjectile::generateTNTMesh()
{
    BlockData *tntData = BlocDatabase::getInstance().getBloc(TNT);
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

void TNTProjectile::onExpire()
{
    // When the TNT expires, it explodes
    explode(getWorldPosition().x, getWorldPosition().y, getWorldPosition().z);
}

void TNTProjectile::clear()
{
    if (m_tntMesh)
    {
        m_tntMesh->cleanupBuffers();
        m_tntMesh->vertices.clear();
        m_tntMesh->triangles.clear();
        m_tntMesh->uvs.clear();
        m_tntMesh->normals.clear();
        m_tntMesh->lights.clear();
        m_tntMesh->ao.clear();
    }
}