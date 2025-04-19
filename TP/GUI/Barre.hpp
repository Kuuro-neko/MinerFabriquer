#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <glm/glm.hpp>


class Barre {
public:
    Barre(int windowWidth, int windowHeight);
    ~Barre();

    void render();
    void updateSize(int windowWidth, int windowHeight);
    void cleanupBuffers();

private:

    std::vector<glm::vec2> slots;

    // VAO : Vertex Array Object — stocke l'état de configuration des attributs de vertex (liaison entre les VBO et les shaders)
    // VBO : Vertex Buffer Object — contient les données des sommets (positions, normales, UVs, etc.)
    // EBO : Element Buffer Object — contient les indices pour le dessin avec des éléments (glDrawElements)

    unsigned int VAO, VBO, EBO;
    GLuint shaderID;

    int m_windowWidth;
    int m_windowHeight;

    void initSlots();
    void initBuffers();
};
