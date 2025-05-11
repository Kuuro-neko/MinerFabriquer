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
        //cleanupBuffers();
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

    // move 
    MeshObject(MeshObject&& other) noexcept : vertexbuffer(other.vertexbuffer), elementbuffer(other.elementbuffer), uvbuffer(other.uvbuffer), normalbuffer(other.normalbuffer), vao(other.vao), vertices(std::move(other.vertices)), triangles(std::move(other.triangles)), uvs(std::move(other.uvs)), normals(std::move(other.normals)) {
        other.vertexbuffer = 0;
        other.elementbuffer = 0;
        other.uvbuffer = 0;
        other.normalbuffer = 0;
        other.vao = 0;
    }

    // move assignment operator
    MeshObject& operator=(MeshObject&& other) noexcept {
        if (this != &other) {
            vertexbuffer = other.vertexbuffer;
            elementbuffer = other.elementbuffer;
            uvbuffer = other.uvbuffer;
            normalbuffer = other.normalbuffer;
            vao = other.vao;
            vertices = std::move(other.vertices);
            triangles = std::move(other.triangles);
            uvs = std::move(other.uvs);
            normals = std::move(other.normals);

            other.vertexbuffer = 0;
            other.elementbuffer = 0;
            other.uvbuffer = 0;
            other.normalbuffer = 0;
            other.vao = 0;
        }
        return *this;
    }

    // delete copy
    MeshObject(const MeshObject&) = delete;
    MeshObject& operator=(const MeshObject&) = delete;
    
};

class VoxelMeshObject : public MeshObject {
    public:
        GLuint lightbuffer;
        GLuint aobuffer;
        std::vector<int> lights;
        std::vector<float> ao;
        VoxelMeshObject() : lightbuffer(0), aobuffer(0), MeshObject() {}
        ~VoxelMeshObject() {}
    
        void initializeBuffers() override;
    
        void draw(GLuint programID) override;

        void cleanupBuffers() override;

        glm::vec3 raycast(Ray ray) override;

        // move constructor
        VoxelMeshObject(VoxelMeshObject&& other) noexcept : MeshObject(std::move(other)), lightbuffer(other.lightbuffer), aobuffer(other.aobuffer), lights(std::move(other.lights)), ao(std::move(other.ao)) {
            other.lightbuffer = 0;
            other.aobuffer = 0;
            other.lights.clear();
            other.ao.clear();
        }

        // move assignment operator
        VoxelMeshObject& operator=(VoxelMeshObject&& other) noexcept {
            if (this != &other) {
                MeshObject::operator=(std::move(other));
                lightbuffer = other.lightbuffer;
                aobuffer = other.aobuffer;
                lights = std::move(other.lights);
                ao = std::move(other.ao);

                other.lightbuffer = 0;
                other.aobuffer = 0;
                other.lights.clear();
                other.ao.clear();
            }
            return *this;
        }

        // delete copy constructor and assignment operator
        VoxelMeshObject(const VoxelMeshObject&) = delete;
        VoxelMeshObject& operator=(const VoxelMeshObject&) = delete;
    };