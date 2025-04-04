#include <TP/Scene/Texture.hpp>
#include <TP/Scene/BlocTypes.hpp>
#include <external/stb_image/stb_image.h>


// ---- Texture ----

Texture::Texture(char* filename) {
    loadTexture(filename);
    setNextFreeBindingIndex();

    genTexture();
}

Texture::Texture(const char* filename) {
    loadTexture(filename);
    setNextFreeBindingIndex();

    genTexture();
}

Texture::Texture(char* filename, int bindingIndex) {
    loadTexture(filename);
    this->bindingIndex = bindingIndex;
}

void Texture::loadTexture(const char* filename) {
    data = stbi_load(filename, &width, &height, &nrChannels, STBI_rgb_alpha);
    if (data) {
        std::cout << "Loaded texture " << filename << " with size " << width << "x" << height << std::endl;
        std::cout << "Nb channels: " << nrChannels << std::endl;
    } else {
        std::cerr << "Failed to load texture " << filename << std::endl;
    }
}

void Texture::genTexture() {
    glGenTextures(1, &handleIndex);
    glBindTexture(GL_TEXTURE_2D, handleIndex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
}

void Texture::bind(GLuint programID) {
    textureID = glGetUniformLocation(programID, samplerName);
    glActiveTexture(GL_TEXTURE0 + bindingIndex);
    glBindTexture(GL_TEXTURE_2D, handleIndex);
    glUniform1i(textureID, bindingIndex);
}