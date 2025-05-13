#include "EnderPearl.hpp"
#include <utils/GLUtils.hpp>
#include "TP/Scene/BlocTypes.hpp"
#include <TP/Scene/World.hpp>

// EnderPearl::EnderPearl(glm::vec3 position, glm::vec3 velocity, float radius, World *world, GLuint programID)
//     : Projectile(position, velocity * ENDERPEARL_PROJ_SPEED, radius, ENDERPEARL_PROJ_SPEED, world, programID)
// {
//     generateMesh();
// }

// EnderPearl::~EnderPearl()
// {
//     clear();
// }

// void EnderPearl::generateMesh()
// {
//     BlockData *tntData = BlocDatabase::getInstance().getBloc(TNT); // TODO : Change to Ender Pearl
//     m_enderPearlMesh = std::make_shared<VoxelMeshObject>();

//     m_enderPearlMesh->vertices.clear();
//     m_enderPearlMesh->triangles.clear();
//     m_enderPearlMesh->uvs.clear();
//     m_enderPearlMesh->normals.clear();
//     m_enderPearlMesh->lights.clear();
//     m_enderPearlMesh->ao.clear();

//     addSquareGeometry(m_enderPearlMesh, TNT, FACE_BOTTOM, 0.f, 0.f, 0.f, false, m_radius);
//     addSquareGeometry(m_enderPearlMesh, TNT, FACE_TOP, 0.f, 0.f, 0.f, false, m_radius);
//     addSquareGeometry(m_enderPearlMesh, TNT, FACE_WEST, 0.f, 0.f, 0.f, false, m_radius);
//     addSquareGeometry(m_enderPearlMesh, TNT, FACE_EAST, 0.f, 0.f, 0.f, false, m_radius);
//     addSquareGeometry(m_enderPearlMesh, TNT, FACE_NORTH, 0.f, 0.f, 0.f, false, m_radius);
//     addSquareGeometry(m_enderPearlMesh, TNT, FACE_SOUTH, 0.f, 0.f, 0.f, false, m_radius);

//     for (int i = 0; i < m_enderPearlMesh->vertices.size(); i++)
//     {
//         m_enderPearlMesh->lights.push_back(15);
//         m_enderPearlMesh->ao.push_back(3);
//     }

//     m_enderPearlMesh->initializeBuffers();
// }

// void EnderPearl::draw(GLuint programID)
// {
//     m_enderPearlMesh->draw(programID);
// }

// void EnderPearl::onExpire()
// {
//     // TODO : Teleport the player to the position of the pearl
//     // m_world->getPlayer()->setPosition(getWorldPosition());
//     m_world->removeTNT(this); // TODO CHANGE NAME
// }

// void EnderPearl::clear()
// {
//     m_enderPearlMesh->clear();
// }
