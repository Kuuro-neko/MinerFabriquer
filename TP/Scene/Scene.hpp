#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include <GL/glew.h>
#include <TP/Scene/BlocTypes.hpp>
#include <TP/Scene/Texture.hpp>
#include <iostream>
#include <utils/GLUtils.hpp>



#define AXIS_X glm::vec3(1.0f, 0.0f, 0.0f)
#define AXIS_Y glm::vec3(0.0f, 1.0f, 0.0f)
#define AXIS_Z glm::vec3(0.0f, 0.0f, 1.0f)

#define DEFAULT_TRANSLATION glm::vec3(0.0f)
#define DEFAULT_ROTATION glm::mat3x3(1.0f)
#define DEFAULT_SCALE 1.0f

class Ray
{
public:
    glm::vec3 origin;
    glm::vec3 direction;
    Ray(glm::vec3 origin, glm::vec3 direction) : origin(origin), direction(direction) {}
    ~Ray() {}
};

class Transform
{
public:
    float m_scale;
    glm::mat3x3 m_rotation;
    glm::vec3 m_translation;
    Transform(glm::vec3 translation = DEFAULT_TRANSLATION, glm::mat3x3 rotation = DEFAULT_ROTATION, float scale = DEFAULT_SCALE)
    : m_scale(scale), m_rotation(rotation), m_translation(translation) {}

    ~Transform() {}

    void rotate(float angle, glm::vec3 axis) {
        m_rotation = glm::mat3x3(glm::rotate(glm::mat4(1.0f), angle, axis));
    }

    void translate(glm::vec3 translation) {
        m_translation += translation;
    }

    void scale(float scale) {
        m_scale *= scale;
    }
};

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

struct Faces {
    unsigned char front : 1;
    unsigned char back : 1;
    unsigned char left : 1;
    unsigned char right : 1;
    unsigned char top : 1;
    unsigned char bottom : 1;

    Faces() {
        front = back = left = right = top = bottom = 0;
    }
};

/*class Voxel : public MeshObject
{
public:
    int m_bloc;
    Texture* m_texture_top;
    Texture* m_texture_side;

    Voxel(int bloc) {
        m_bloc = bloc;
        m_texture_top = BlocTextureDatabase::getInstance().getTexture(m_bloc)->getTexture(BLOC_TOP);
        m_texture_side = BlocTextureDatabase::getInstance().getTexture(m_bloc)->getTexture(BLOC_LEFT);
    }

    void draw(GLuint programID) override;

    void createCubeGeometry(unsigned char faces, float x_offset = 0.0f, float y_offset = 0.0f, float z_offset = 0.0f, float size = 1.0f) {
        vertices.clear();
        triangles.clear();
        uvs.clear();
        addSquareGeometry(vertices, triangles, uvs, faces);
        initializeBuffers();
    }
};*/

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
    void rotate(float angle, glm::vec3 axis);

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

class VoxelChunk : public SceneNode
{
public:
    int m_sizeX;
    int m_sizeY;
    int m_sizeZ;
    int*** m_cubes;

    VoxelChunk(int sizeX, int sizeY, int sizeZ, Transform t, MeshObject* m) : SceneNode(t, m, nullptr), m_sizeX(sizeX), m_sizeY(sizeY), m_sizeZ(sizeZ) {
        m_cubes = new int**[m_sizeX];
        for (int i = 0; i < m_sizeX; i++) {
            m_cubes[i] = new int*[m_sizeY];
            for (int j = 0; j < m_sizeY; j++) {
                m_cubes[i][j] = new int[m_sizeZ];
                for (int k = 0; k < m_sizeZ; k++) {
                    m_cubes[i][j][k] = AIR;
                }
            }
        }
    }
    ~VoxelChunk() {
        for (int i = 0; i < m_sizeX; i++) {
            for (int j = 0; j < m_sizeY; j++) {
                delete[] m_cubes[i][j];
            }
            delete[] m_cubes[i];
        }
        delete[] m_cubes;
    }
    void setBloc(int x, int y, int z, int bloc) {
        m_cubes[x][y][z] = bloc;
    }
    int getBloc(int x, int y, int z) {
        return m_cubes[x][y][z];
    }

    void generateMesh() {
        // Clear the mesh
        m_mesh->vertices.clear();
        m_mesh->triangles.clear();
        m_mesh->uvs.clear();

        // Loop through all the cubes in the chunk. If a cube has a face that is not adjacent to another cube, add a face to the mesh
        for (int x = 0; x < m_sizeX; x++) {
            for (int y = 0; y < m_sizeY; y++) {
                for (int z = 0; z < m_sizeZ; z++) {
                    if (m_cubes[x][y][z] != AIR) {
                        // Check all the faces of the cube
                        bool test = false;
                        if (x == 0 || m_cubes[x - 1][y][z] == AIR || test) {
                            addSquareGeometry(m_mesh->vertices, m_mesh->triangles, m_mesh->uvs, m_cubes[x][y][z], BLOC_LEFT, x, y, z);
                        }
                        if (x == m_sizeX - 1 || m_cubes[x + 1][y][z] == AIR || test) {
                            addSquareGeometry(m_mesh->vertices, m_mesh->triangles, m_mesh->uvs, m_cubes[x][y][z], BLOC_RIGHT, x, y, z);
                        }
                        if (y == 0 || m_cubes[x][y - 1][z] == AIR || test) {
                            addSquareGeometry(m_mesh->vertices, m_mesh->triangles, m_mesh->uvs, m_cubes[x][y][z], BLOC_BOTTOM, x, y, z);
                        }
                        if (y == m_sizeY - 1 || m_cubes[x][y + 1][z] == AIR || test) {
                            addSquareGeometry(m_mesh->vertices, m_mesh->triangles, m_mesh->uvs, m_cubes[x][y][z], BLOC_TOP, x, y, z);
                        }
                        if (z == 0 || m_cubes[x][y][z - 1] == AIR || test) {
                            addSquareGeometry(m_mesh->vertices, m_mesh->triangles, m_mesh->uvs, m_cubes[x][y][z], BLOC_FRONT, x, y, z);
                        }
                        if (z == m_sizeZ - 1 || m_cubes[x][y][z + 1] == AIR || test) {
                            addSquareGeometry(m_mesh->vertices, m_mesh->triangles, m_mesh->uvs, m_cubes[x][y][z], BLOC_BACK, x, y, z);
                        }
                    }
                }
            }
        }
        // Initialize the buffers for the mesh
        m_mesh->initializeBuffers();
    }

    void draw(GLuint programID) override {
        GLuint modelMatrixId = glGetUniformLocation(programID, "ModelMatrix");
        glUniformMatrix4fv(modelMatrixId, 1, false, &ModelMatrix[0][0]);
        
        TextureAtlas::getInstance().bind(programID);
        m_mesh->draw(programID);
    }

    void cleanupBuffers() override {
        m_mesh->cleanupBuffers();
    }
private:

};