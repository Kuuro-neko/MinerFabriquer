#pragma once

#include <TP/Scene/SceneNode.hpp>

class Skybox : public SceneNode {
    Skybox() : SceneNode(Transform(
        glm::vec3(0, 0, 0),
        DEFAULT_ROTATION,
        1.f
    ), nullptr, nullptr) {
        m_texture = TextureSkybox::getInstance().getTexture();
    }
    

    void rotate(float angle, glm::vec3 axis) override{
        // Don't rotate the skybox
    }

    void generateBuffers() {
        m_mesh = new MeshObject();
    
    }
}