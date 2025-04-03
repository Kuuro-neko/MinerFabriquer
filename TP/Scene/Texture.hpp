#pragma once

#include <common/imageLoader.h>
#include <GL/glew.h>
#include <TP/Scene/BlocTypes.hpp>

#define TEXTUREATLAS_COORD_UNIT 0.0625f

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
    char* samplerName = "TextureSampler";
    GLuint handleIndex;
    GLuint bindingIndex;
    GLuint textureID;
    GLuint format = GL_RGB; // could be GL_RGBA
};

/**
 * @brief TextureAtlas is a singleton class that loads the texture atlas
 * 
 */
class TextureAtlas {
private:
    TextureAtlas() {
        m_texture = Texture("../textures/texture_atlas.ppm");
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