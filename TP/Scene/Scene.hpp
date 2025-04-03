#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include <GL/glew.h>
#include "BlocTypes.hpp"
#include <iostream>

#include <common/imageLoader.h>

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


#define DONT_INCREMENT_BINDING 0

class NextFreeIndexBinding {
private:
    int index;
    NextFreeIndexBinding() : index(0) {}
public:
    static NextFreeIndexBinding& getInstance() {
        static NextFreeIndexBinding instance;
        return instance;
    }
    int get_next_free_binding_index() {
        return index++;
    }
};

class Texture{
public:
    ppmLoader::ImageRGB image;
    /**
     * @brief Construct a new Texture object with empty texture image, and assigns it the next free binding index
     * 
     */
    Texture() {
        setNextFreeBindingIndex();
    }

    /**
     * @brief Construct a new Texture object, and assigns it the next free binding index
     * 
     * @param filename image filename
     */
    Texture(char* filename);

        /**
     * @brief Construct a new Texture object, and assigns it the next free binding index
     * 
     * @param filename image filename
     */
    Texture(const char* filename);

    /**
     * @brief Construct a new Texture object
     * 
     * @param filename image filename
     * @param bindingIndex binding index of the texture, can be set to DONT_INCREMENT_BINDING to not increment the binding index and use it without a binding index
     */
    Texture(char* filename, int bindingIndex);

    /**
     * @brief Generate texture buffer and set texture parameters
     * 
     */
    void genTexture();

    /**
     * @brief Binds this texture to the given GLSL program
     * 
     * @param programID 
     */
    void bind(GLuint programID);

    /**
     * @brief Set the binding index of this texture to the next free binding index using NextFreeIndexBinding singleton
     * 
     */
    void setNextFreeBindingIndex() {
        bindingIndex = NextFreeIndexBinding::getInstance().get_next_free_binding_index();
    }

    void setSamplerName(char* samplerName) {
        this->samplerName = samplerName;
    }
private:
    char* samplerName = "PlanetTextureSampler";
    GLuint handleIndex;
    GLuint bindingIndex;
    GLuint textureID;
    GLuint format = GL_RGB; // could be GL_RGBA
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

class Voxel : public MeshObject
{
public:
    int m_bloc;
    Texture m_texture;

    Voxel(int bloc) {
        m_bloc = bloc;
        m_texture = Texture(getTexturePath(bloc, BLOC_SIDE).c_str());
        createCubeGeometry();
        initializeBuffers();
    }

    void draw(GLuint programID) override;

    void createCubeGeometry() {
        vertices.clear();
        triangles.clear();
        uvs.clear();
        // Front face
        vertices.push_back(glm::vec3(0.f, 0.f, 0.f));
        vertices.push_back(glm::vec3(1.f, 0.f, 0.f));
        vertices.push_back(glm::vec3(1.f, 1.f, 0.f));
        vertices.push_back(glm::vec3(0.f, 1.f, 0.f));
        triangles.push_back(0);
        triangles.push_back(1);
        triangles.push_back(2);
        triangles.push_back(0);
        triangles.push_back(2);
        triangles.push_back(3);
        // Left face
        vertices.push_back(glm::vec3(0.f, 0.f, 0.f));
        vertices.push_back(glm::vec3(0.f, 0.f, 1.f));
        vertices.push_back(glm::vec3(0.f, 1.f, 1.f));
        vertices.push_back(glm::vec3(0.f, 1.f, 0.f));
        triangles.push_back(4);
        triangles.push_back(5);
        triangles.push_back(6);
        triangles.push_back(4);
        triangles.push_back(6);
        triangles.push_back(7);
        // Back face
        vertices.push_back(glm::vec3(0.f, 0.f, 1.f));
        vertices.push_back(glm::vec3(1.f, 0.f, 1.f));
        vertices.push_back(glm::vec3(1.f, 1.f, 1.f));
        vertices.push_back(glm::vec3(0.f, 1.f, 1.f));
        triangles.push_back(8);
        triangles.push_back(9);
        triangles.push_back(10);
        triangles.push_back(8);
        triangles.push_back(10);
        triangles.push_back(11);
        // Right face
        vertices.push_back(glm::vec3(1.f, 0.f, 0.f));
        vertices.push_back(glm::vec3(1.f, 0.f, 1.f));
        vertices.push_back(glm::vec3(1.f, 1.f, 1.f));
        vertices.push_back(glm::vec3(1.f, 1.f, 0.f));
        triangles.push_back(12);
        triangles.push_back(13);
        triangles.push_back(14);
        triangles.push_back(12);
        triangles.push_back(14);
        triangles.push_back(15);
        // Top face
        vertices.push_back(glm::vec3(0.f, 1.f, 0.f));
        vertices.push_back(glm::vec3(1.f, 1.f, 0.f));
        vertices.push_back(glm::vec3(1.f, 1.f, 1.f));
        vertices.push_back(glm::vec3(0.f, 1.f, 1.f));
        triangles.push_back(16);
        triangles.push_back(17);
        triangles.push_back(18);
        triangles.push_back(16);
        triangles.push_back(18);
        triangles.push_back(19);
        // Bottom face
        vertices.push_back(glm::vec3(0.f, 0.f, 0.f));
        vertices.push_back(glm::vec3(1.f, 0.f, 0.f));
        vertices.push_back(glm::vec3(1.f, 0.f, 1.f));
        vertices.push_back(glm::vec3(0.f, 0.f, 1.f));
        triangles.push_back(20);
        triangles.push_back(21);
        triangles.push_back(22);
        triangles.push_back(20);
        triangles.push_back(22);
        triangles.push_back(23);
        // UVs
        for (int i = 0; i < 6; i++) {
            uvs.push_back(glm::vec2(0.f, 0.f));
            uvs.push_back(glm::vec2(1.f, 0.f));
            uvs.push_back(glm::vec2(1.f, 1.f));
            uvs.push_back(glm::vec2(0.f, 1.f));
        }
    }
};

class SceneNode
{
public:
    MeshObject* m_mesh;
    Texture* m_texture;
    Transform m_transform;
    
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
    void draw(GLuint programID);

    /**
     * @brief Clean the buffers of this scene node and all its children recursively
     * 
     */
    void cleanupBuffers();

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
    glm::mat4 ModelMatrix;
};