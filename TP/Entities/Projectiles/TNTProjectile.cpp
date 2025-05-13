#include "TNTProjectile.hpp"
#include <TP/Database/BlocTypes.hpp>
#include <utils/GLUtils.hpp>

TNTProjectile::TNTProjectile(glm::vec3 position, glm::vec3 velocity, float radius, World *world, GLuint programID)
    : Projectile(position, velocity * TNT_PROJ_SPEED, radius, TNT_PROJ_SPEED, world, programID)
{
    generateMesh();
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

    m_world->removeTNT(this, m_explosionRadius);  // TODO Changed from removeTNT to more generic removeProjectile
}

void TNTProjectile::generateMesh()
{
    if (!m_mesh)
    {
        m_mesh = std::make_shared<VoxelMeshObject>();
    }

    m_mesh->vertices.clear();
    m_mesh->triangles.clear();
    m_mesh->uvs.clear();
    m_mesh->normals.clear();
    m_mesh->lights.clear();
    m_mesh->ao.clear();

    addSquareGeometry(m_mesh, TNT, FACE_BOTTOM, 0.f, 0.f, 0.f, false, m_radius);
    addSquareGeometry(m_mesh, TNT, FACE_TOP, 0.f, 0.f, 0.f, false, m_radius);
    addSquareGeometry(m_mesh, TNT, FACE_WEST, 0.f, 0.f, 0.f, false, m_radius);
    addSquareGeometry(m_mesh, TNT, FACE_EAST, 0.f, 0.f, 0.f, false, m_radius);
    addSquareGeometry(m_mesh, TNT, FACE_NORTH, 0.f, 0.f, 0.f, false, m_radius);
    addSquareGeometry(m_mesh, TNT, FACE_SOUTH, 0.f, 0.f, 0.f, false, m_radius);

    for (int i = 0; i < m_mesh->vertices.size(); i++)
    {
        m_mesh->lights.push_back(15);
        m_mesh->ao.push_back(3);
    }

    m_mesh->initializeBuffers();
}

void TNTProjectile::draw(GLuint programID)
{
    TextureManager::getInstance().getPBRTexture("blocks")->bind(programID);
    GLuint modelMatrixId = glGetUniformLocation(programID, "ModelMatrix");
    glUniformMatrix4fv(modelMatrixId, 1, false, &ModelMatrix[0][0]);
    m_mesh->draw(programID);
}

void TNTProjectile::onExpire()
{
    // When the TNT expires, it explodes
    explode(getWorldPosition().x, getWorldPosition().y, getWorldPosition().z);
}

void TNTProjectile::clear()
{
    if (m_mesh)
    {
        m_mesh->cleanupBuffers();
        m_mesh->vertices.clear();
        m_mesh->triangles.clear();
        m_mesh->uvs.clear();
        m_mesh->normals.clear();
        m_mesh->lights.clear();
        m_mesh->ao.clear();
    }
}