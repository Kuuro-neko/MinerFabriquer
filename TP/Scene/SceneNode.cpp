#include <TP/Scene/SceneNode.hpp>

void SceneNode::addChild(SceneNode* child) {
    m_children.push_back(child);
    child->m_parent = this;
}

void SceneNode::removeChild(SceneNode* child) {
    for (int i = 0; i < m_children.size(); i++) {
        if (m_children[i] == child) {
            m_children.erase(m_children.begin() + i);
            child->m_parent = nullptr;
            return;
        }
    }
}

void SceneNode::updateModelMatrix() {
    ModelMatrix = glm::mat4(1.0f);
    ModelMatrix = glm::translate(ModelMatrix, m_transform.m_translation);
    ModelMatrix = ModelMatrix * glm::mat4(m_transform.m_rotation);
    ModelMatrix = glm::scale(ModelMatrix, glm::vec3(m_transform.m_scale));

    if (m_parent) {
        ModelMatrix = m_parent->ModelMatrix * ModelMatrix;
    }

    for (int i = 0; i < m_children.size(); i++) {
        m_children[i]->updateModelMatrix();
    }
}

void SceneNode::draw(GLuint programID) {
    if (m_mesh) {
        GLuint modelMatrixId = glGetUniformLocation(programID, "ModelMatrix");
        glUniformMatrix4fv(modelMatrixId, 1, false, &ModelMatrix[0][0]);
        
        if (m_texture) {
            m_texture->bind(programID);
        }
        
        m_mesh->draw(programID);
    }
    for (int i = 0; i < m_children.size(); i++) {
        m_children[i]->draw(programID);
    }
}

void SceneNode::cleanupBuffers() {
    if (m_mesh) {
        m_mesh->cleanupBuffers();
    }
    for (int i = 0; i < m_children.size(); i++) {
        m_children[i]->cleanupBuffers();
    }
}

void SceneNode::rotate(float angle, glm::vec3 axis) {
    m_transform.rotate(angle, axis);
    updateModelMatrix();
}

void SceneNode::translate(glm::vec3 translation) {
    m_transform.translate(translation);
    updateModelMatrix();
}

void SceneNode::scale(float scale) {
    m_transform.scale(scale);
    updateModelMatrix();
}