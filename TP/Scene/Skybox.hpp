#pragma once

#include <TP/Scene/SceneNode.hpp>
#include <utils/GLUtils.hpp>

struct SkyboxTexCoords {
    float unit_x = 0.25f;
    float unit_y = 1.f/3.f;

    float top_x = 1.f * unit_x;
    float top_y = 0.f;

    float left_x = 0.f;
    float left_y = 1.f * unit_y;

    float front_x = 1.f * unit_x;
    float front_y = 1.f * unit_y;

    float right_x = 2.f * unit_x;
    float right_y = 1.f * unit_y;

    float back_x = 3.f * unit_x;
    float back_y = 1.f * unit_y;

    float bottom_x = 1.f * unit_x;
    float bottom_y = 2.f * unit_y;
};

class Skybox : public SceneNode {
public:
    Skybox() : SceneNode(Transform(
        glm::vec3(0, 0, 0),
        DEFAULT_ROTATION,
        1.f
    ), nullptr, nullptr) {
        m_texture = TextureSkybox::getInstance().getTexture();
        generateBuffers();
    }
    

    void rotate(float angle, glm::vec3 axis) override{
        // Don't rotate the skybox
    }

    void generateBuffers() {
        m_mesh = new MeshObject();
        addSkyboxGeometry(m_mesh->vertices, m_mesh->triangles, m_mesh->uvs);
        m_mesh->initializeBuffers();
    }
};

