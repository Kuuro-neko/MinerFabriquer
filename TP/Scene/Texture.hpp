#pragma once

#include <stb_image.h>
#include <GL/glew.h>
#include <TP/Scene/BlocTypes.hpp>
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

/**
 * @brief TextureAtlas is a singleton class that loads the texture atlas
 * 
 */
class TextureAtlas {
private:
    TextureAtlas() {
        m_texture = Texture("../textures/texture_atlas.png");
        m_texture.genTexture();
    }
    Texture m_texture;
public:
    static TextureAtlas& getInstance() {
        static TextureAtlas instance;
        return instance;
    }

    /**
     * @brief Get the Texture object
     * 
     * @return Texture* 
     */
    Texture* getTexture() {
        return &m_texture;
    }

    /**
     * @brief Bind the texture atlas to the given GLSL program
     * 
     * @param programID 
     */
    void bind(GLuint programID) {
        m_texture.bind(programID);
    }
};

class PBRTextureAtlas {
private:
    PBRTextureAtlas(int resolution = 32) {
        std::string resolutionStr = std::to_string(resolution);
        m_texture = Texture(("../textures/texture_atlas_x" + resolutionStr + ".png").c_str());
        m_texture.genTexture();
        m_texture.setSamplerName("TextureSampler");

        m_normals = Texture(("../textures/normal_atlas_x" + resolutionStr + ".png").c_str());
        m_normals.genTexture();
        m_normals.setSamplerName("NormalsSampler");

        m_roughness = Texture(("../textures/roughness_atlas_x" + resolutionStr + ".png").c_str());
        m_roughness.genTexture();
        m_roughness.setSamplerName("RoughnessSampler");

        m_metallic = Texture(("../textures/metallic_atlas_x" + resolutionStr + ".png").c_str());
        m_metallic.genTexture();
        m_metallic.setSamplerName("MetallicSampler");
    }
    Texture m_texture;
    Texture m_normals;
    Texture m_roughness;
    Texture m_metallic;
public:
    static PBRTextureAtlas& getInstance() {
        static PBRTextureAtlas instance;
        return instance;
    }

    /**
     * @brief Get the Texture object
     * 
     * @return Texture* 
     */
    Texture* getTexture() {
        return &m_texture;
    }

    Texture* getNormals() {
        return &m_normals;
    }

    Texture* getRoughness() {
        return &m_roughness;
    }

    Texture* getMetallic() {
        return &m_metallic;
    }

    /**
     * @brief Bind the texture atlas to the given GLSL program
     * 
     * @param programID 
     */
    void bind(GLuint programID) {
        m_texture.bind(programID);
        m_normals.bind(programID);
        m_roughness.bind(programID);
        m_metallic.bind(programID);
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