#pragma once

#include <stb_image.h>
#include <GL/glew.h>
#include <TP/Database/BlocTypes.hpp>
#include <TP/Camera/Camera.hpp>

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
    unsigned char * data = nullptr;
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


    void loadTexture(const char* filename);

    /**
     * @brief Generate texture buffer and set texture parameters
     * 
     */
    void genTexture(GLenum wrapS = GL_CLAMP_TO_EDGE, GLenum wrapT = GL_CLAMP_TO_EDGE, GLenum minFilter = GL_NEAREST, GLenum magFilter = GL_NEAREST);

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
    char* samplerName = "TextureSampler";
    GLuint handleIndex;
    GLuint bindingIndex;
    GLuint textureID;
    GLuint format = GL_RGBA;
    int width, height, nrChannels = 0;
};

class PBRTexture : Texture {
private:
    Texture m_texture;
    Texture m_normals;
    Texture m_roughness;
    Texture m_metallic;
public:
    PBRTexture(std::string textureName, std::string normalName, std::string roughnessName, std::string metallicName) {
        m_texture = Texture(textureName.c_str());
        m_texture.genTexture();
        m_texture.setSamplerName("TextureSampler");

        m_normals = Texture(normalName.c_str());
        m_normals.genTexture();
        m_normals.setSamplerName("NormalsSampler");

        m_roughness = Texture(roughnessName.c_str());
        m_roughness.genTexture();
        m_roughness.setSamplerName("RoughnessSampler");

        m_metallic = Texture(metallicName.c_str());
        m_metallic.genTexture();
        m_metallic.setSamplerName("MetallicSampler");
    }

    void bind(GLuint programID) {
        m_texture.bind(programID);
        m_normals.bind(programID);
        m_roughness.bind(programID);
        m_metallic.bind(programID);
    }

    void bindOnlyTexture(GLuint programID) {
        m_texture.bind(programID);
    }
};

class CubemapTexture {
private:
    
    GLuint textureID, VAO, VBO;
    float skyboxVertices[108];
    int programID;
public:
    CubemapTexture(int programID);
    inline void setProgramID(int programID) {
        this->programID = programID;
    }
    void draw(Camera &camera);
    void cleanupBuffers();
};