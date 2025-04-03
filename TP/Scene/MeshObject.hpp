#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <utils/Ray.hpp>

class MeshObject
{
public:
    GLuint vertexbuffer;
    GLuint elementbuffer;
    GLuint uvbuffer;
    std::vector<glm::vec3> vertices;
    std::vector<unsigned short> triangles;
    std::vector<glm::vec2> uvs;

    MeshObject() : vertexbuffer(0), elementbuffer(0), uvbuffer(0) {}
    ~MeshObject() {
        cleanupBuffers();
    }

    /**
     * @brief Initialize the buffers for the mesh object (vertices, triangles, uvs)
     * 
     */
    void initializeBuffers();

    /**
     * @brief Draw the mesh object
     * 
     */
    virtual void draw(GLuint programID);

    /**
     * @brief Clean the buffers for the mesh object
     * 
     */
    void cleanupBuffers();

    glm::vec3 raycast(Ray ray);
};