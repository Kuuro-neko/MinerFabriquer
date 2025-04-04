#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include <GL/glew.h>
#include <TP/Scene/BlocTypes.hpp>
#include <TP/Scene/Texture.hpp>
#include <TP/Scene/MeshObject.hpp>
#include <iostream>
#include <utils/GLUtils.hpp>
#include <utils/Ray.hpp>
#include <utils/Transform.hpp>

class SceneNode
{
public:
    MeshObject* m_mesh;
    Texture* m_texture;
    Transform m_transform;
    glm::mat4 ModelMatrix;

    SceneNode(
        Transform transform = Transform(),
        MeshObject* mesh = nullptr,
        Texture* texture = nullptr) : 
        m_transform(transform),
        m_mesh(mesh),
        m_texture(texture),
        m_parent(nullptr),
        m_children(std::vector<SceneNode*>()) {
            updateModelMatrix();
        }
    ~SceneNode() {}

    /**
     * @brief Add a child to this scene node. The child's parent is set to this node.
     * 
     * @param child 
     */
    void addChild(SceneNode* child);

    /**
     * @brief Remove a child from this scene node. The child's parent is set to nullptr.
     * 
     * @param child 
     */
    void removeChild(SceneNode* child);

    /**
     * @brief Update the model matrix of this scene node and all its children recursively
     * 
     */
    void updateModelMatrix();

    /**
     * @brief Draws this scene node and all its children recursively
     * 
     * @param programID 
     */
    virtual void draw(GLuint programID);

    /**
     * @brief Clean the buffers of this scene node and all its children recursively
     * 
     */
    virtual void cleanupBuffers();

    /**
     * @brief Apply a rotation to this scene node around the given axis, and update the model matrix. Also applies the rotation to all children.
     * 
     * @param angle 
     * @param axis 
     */
    virtual void rotate(float angle, glm::vec3 axis);

    /**
     * @brief Apply a translation to this scene node, and update the model matrix. Also applies the translation to all children.
     * 
     * @param translation 
     */
    void translate(glm::vec3 translation);

    /**
     * @brief Apply a scale to this scene node, and update the model matrix. Also applies the scale to all children.
     * 
     * @param scale 
     */
    void scale(float scale);

    glm::vec3 getWorldPosition() {
        return glm::vec3(ModelMatrix[3]);
    }


    void keepAboveGround(SceneNode* ground) {
        Ray ray = Ray(getWorldPosition(), glm::vec3(0.0f, -1.0f, 0.0f));
        glm::vec3 hit = ground->m_mesh->raycast(ray);
        translate(hit - getWorldPosition() + glm::vec3(0.0f, 1.f, 0.0f));
    }

private:
    SceneNode* m_parent;
    std::vector<SceneNode*> m_children;
};