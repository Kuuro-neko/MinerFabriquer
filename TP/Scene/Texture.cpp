#include <TP/Scene/Texture.hpp>
#include <TP/Scene/BlocTypes.hpp>
#include <common/imageLoader.h>

// ---- Texture ----

Texture::Texture(char* filename) {
    ppmLoader::load_ppm(image, filename);
    setNextFreeBindingIndex();

    genTexture();
}

Texture::Texture(const char* filename) {
    ppmLoader::load_ppm(image, filename);
    setNextFreeBindingIndex();

    genTexture();
}

Texture::Texture(char* filename, int bindingIndex) {
    ppmLoader::load_ppm(image, filename);
    this->bindingIndex = bindingIndex;
}

void Texture::genTexture() {
    glGenTextures(1, &handleIndex);
    glBindTexture(GL_TEXTURE_2D, handleIndex);
    glTexImage2D(GL_TEXTURE_2D, 0, format, image.w, image.h, 0, format, GL_UNSIGNED_BYTE, &image.data[0]);
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