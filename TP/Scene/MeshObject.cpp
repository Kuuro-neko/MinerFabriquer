// ---- MeshObject ----

#include <TP/Scene/MeshObject.hpp>

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
