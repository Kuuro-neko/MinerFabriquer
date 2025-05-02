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
    GLuint normalbuffer;
    GLuint vao;
    std::vector<glm::vec3> vertices;
    std::vector<unsigned short> triangles;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;

    MeshObject() : vertexbuffer(0), elementbuffer(0), uvbuffer(0), normalbuffer(0), vao(0) {}
    ~MeshObject() {
        cleanupBuffers();
    }

    /**
     * @brief Initialize the buffers for the mesh object (vertices, triangles, uvs)
     * 
     */
    virtual void initializeBuffers();

    /**
     * @brief Draw the mesh object
     * 
     */
    virtual void draw(GLuint programID);

    /**
     * @brief Clean the buffers for the mesh object
     * 
     */
    virtual void cleanupBuffers();

    virtual glm::vec3 raycast(Ray ray);
};

class VoxelMeshObject : public MeshObject {
    public:
        GLuint lightbuffer;
        std::vector<unsigned short> lights;
        VoxelMeshObject() : lightbuffer(0), MeshObject() {}
        ~VoxelMeshObject() {}
    
        void initializeBuffers() override;
    
        void draw(GLuint programID) override;

        void cleanupBuffers() override;

        glm::vec3 raycast(Ray ray) override;
    };