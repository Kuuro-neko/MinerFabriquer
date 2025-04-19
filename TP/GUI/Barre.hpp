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

private:
    struct Slot {
        float x, y;
    };

    std::vector<Slot> slots;

    unsigned int VAO, VBO, EBO;
    GLuint shaderID;

    int m_windowWidth;
    int m_windowHeight;

    void initSlots();
    void initBuffers();
};
