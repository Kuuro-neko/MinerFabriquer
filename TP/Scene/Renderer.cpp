#include "Renderer.hpp"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

Renderer::Renderer(GLuint programID) : programID(programID) {
    // Define the vertices for a cube
    vertices = {
            {0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0}, // Front face
            {0, 0, 1}, {1, 0, 1}, {1, 1, 1}, {0, 1, 1}  // Back face
    };

    // Define the line indices for the wireframe
    lines = {
            0, 1, 1, 2, 2, 3, 3, 0, // Front face edges
            4, 5, 5, 6, 6, 7, 7, 4, // Back face edges
            0, 4, 1, 5, 2, 6, 3, 7  // Connecting edges
    };

    initializeBuffers();
}

Renderer::Renderer() {}

void Renderer::initializeBuffers() {
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, lines.size() * sizeof(unsigned short), lines.data(), GL_STATIC_DRAW);
}

void Renderer::drawWireframeCube(
        const glm::vec3 &position,
        const glm::vec3 &size,
        const glm::vec3 &color,
        const glm::mat4 &viewMatrix,
        const glm::mat4 &projectionMatrix
) const {
    glm::mat4 ModelMatrix = glm::translate(glm::mat4(1.0f), position);
    ModelMatrix = glm::scale(ModelMatrix, size);

    glUseProgram(programID);

    // Set uniforms
    glUniformMatrix4fv(glGetUniformLocation(programID, "ModelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(programID, "ViewMatrix"), 1, GL_FALSE, &viewMatrix[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(programID, "ProjectionMatrix"), 1, GL_FALSE, &projectionMatrix[0][0]);

    glUniform3fv(glGetUniformLocation(programID, "color"), 1, &color[0]);

    // Bind vertex positions (location = 0)
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

    // Bind indices and draw
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glDrawElements(GL_LINES, lines.size(), GL_UNSIGNED_SHORT, (void *)0);

    glDisableVertexAttribArray(0);
}