#pragma once

#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "common/shader.hpp"

class Crosshair {
private:
    GLuint VAO, VBO;
    bool visible;
    GLuint crosshairID;

    std::vector<float> vertices;
public:
    void render();
    void initializeBuffers();
    Crosshair(float size) {
        vertices = {
            0.0f, size*16.0f/9.0f,
            0.0f, -size*16.0f/9.0f,

            -size, 0.0f,
            size, 0.0f
        };
        visible = true;

        crosshairID = LoadShaders("vertex_shader_2D.glsl", "fragment_shader_crosshair.glsl");
        
        initializeBuffers();
    }
    ~Crosshair() {
        cleanupBuffers();
        glDeleteProgram(crosshairID);
    }
    void cleanupBuffers();

};