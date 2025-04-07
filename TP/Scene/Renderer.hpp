#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>

class Renderer {
private:
    GLuint programID;
    GLuint vertexbuffer;
    GLuint elementbuffer;
    GLuint vao;

    std::vector<glm::vec3> vertices;
    std::vector<unsigned short> lines;

    glm::vec3 highlight;
    bool highlightEnabled = false;

    void initializeBuffers();

public:
    Renderer(GLuint programID);
    Renderer();
    ~Renderer() {
        glDeleteBuffers(1, &vertexbuffer);
        glDeleteBuffers(1, &elementbuffer);
        glDeleteVertexArrays(1, &vao);
    }

    void drawWireframeCube(const glm::vec3& size, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) const;
    void setHighlight(const glm::vec3& highlight);
    void disableHighlight();
};