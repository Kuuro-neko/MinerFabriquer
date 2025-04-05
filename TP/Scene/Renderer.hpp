#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>

class Renderer {
private:
    GLuint programID;
    GLuint vertexbuffer;
    GLuint elementbuffer;

    std::vector<glm::vec3> vertices;
    std::vector<unsigned short> lines;

    void initializeBuffers();

public:
    Renderer(GLuint programID);
    Renderer();

    void drawWireframeCube(const glm::vec3& position, const glm::vec3& size, const glm::vec3& color, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) const;
};