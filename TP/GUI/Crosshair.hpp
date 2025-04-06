#pragma once

#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Crosshair {
private:
    GLuint VAO, VBO;
    bool visible;
    int programID;

    std::vector<float> vertices;
public:
    void render();
    void initializeBuffers();
    Crosshair(int programID, float size) : programID(programID) {
        vertices = {
            0.0f, size,
            0.0f, -size,

            -size, 0.0f,
            size , 0.0f
        };
        visible = true;
        
        initializeBuffers();
    }
    ~Crosshair() {
        cleanupBuffers();
    }
    void cleanupBuffers();

};