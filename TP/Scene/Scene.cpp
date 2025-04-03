#include <TP/Scene/Scene.hpp>

// ---- Texture ----

Texture::Texture(char* filename) {
    ppmLoader::load_ppm(image, filename);
    setNextFreeBindingIndex();

    genTexture();
}

Texture::Texture(const char* filename) {
    ppmLoader::load_ppm(image, filename);
    setNextFreeBindingIndex();

    genTexture();
}

Texture::Texture(char* filename, int bindingIndex) {
    ppmLoader::load_ppm(image, filename);
    this->bindingIndex = bindingIndex;
}

void Texture::genTexture() {
    glGenTextures(1, &handleIndex);
    glBindTexture(GL_TEXTURE_2D, handleIndex);
    glTexImage2D(GL_TEXTURE_2D, 0, format, image.w, image.h, 0, format, GL_UNSIGNED_BYTE, &image.data[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Texture::bind(GLuint programID) {
    textureID = glGetUniformLocation(programID, samplerName);
    glActiveTexture(GL_TEXTURE0 + bindingIndex);
    glBindTexture(GL_TEXTURE_2D, handleIndex);
    glUniform1i(textureID, bindingIndex);
}

// ---- MeshObject ----

void MeshObject::initializeBuffers() {
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangles.size() * sizeof(unsigned short), &triangles[0], GL_STATIC_DRAW);

    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
}

void MeshObject::draw(GLuint programID) {
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glDrawElements(GL_TRIANGLES, triangles.size(), GL_UNSIGNED_SHORT, (void*)0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

void MeshObject::cleanupBuffers() {
    if (vertexbuffer) {
        glDeleteBuffers(1, &vertexbuffer);
        vertexbuffer = 0;
    }
    if (elementbuffer) {
        glDeleteBuffers(1, &elementbuffer);
        elementbuffer = 0;
    }
    if (uvbuffer) {
        glDeleteBuffers(1, &uvbuffer);
        uvbuffer = 0;
    }
}

glm::vec3 MeshObject::raycast(Ray ray) {
    for (int i = 0; i < triangles.size(); i += 3) {
        glm::vec3 v0 = vertices[triangles[i]];
        glm::vec3 v1 = vertices[triangles[i + 1]];
        glm::vec3 v2 = vertices[triangles[i + 2]];

        glm::vec3 e1 = v1 - v0;
        glm::vec3 e2 = v2 - v0;

        glm::vec3 p = glm::cross(ray.direction, e2);
        float det = glm::dot(e1, p);

        if (det > -0.00001 && det < 0.00001) {
            continue;
        }

        float invDet = 1.0f / det;
        glm::vec3 t = ray.origin - v0;
        float u = glm::dot(t, p) * invDet;

        if (u < 0 || u > 1) {
            continue;
        }

        glm::vec3 q = glm::cross(t, e1);
        float v = glm::dot(ray.direction, q) * invDet;

        if (v < 0 || u + v > 1) {
            continue;
        }

        float t0 = glm::dot(e2, q) * invDet;

        return ray.origin + ray.direction * t0;
    }
    return glm::vec3(0.0f);
}

void Voxel::draw(GLuint programID) {
    m_texture.bind(programID);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glDrawElements(GL_TRIANGLES, triangles.size(), GL_UNSIGNED_SHORT, (void*)0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

// ---- SceneNode ----

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