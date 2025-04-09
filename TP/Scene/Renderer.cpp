#include "Renderer.hpp"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

Renderer::Renderer(GLuint programID) : programID(programID) {
    // Define the vertices for a cube
    vertices = {
        glm::vec3(0.f, 0.f, 0.f),
        glm::vec3(1.f, 0.f, 0.f),
        glm::vec3(1.f, 1.f, 0.f),
        glm::vec3(0.f, 1.f, 0.f),
        glm::vec3(0.f, 0.f, 1.f),
        glm::vec3(1.f, 0.f, 1.f),
        glm::vec3(1.f, 1.f, 1.f),
        glm::vec3(0.f, 1.f, 1.f)
    };

    // Define the lines that connect the vertices to form a wireframe cube
    lines = {
        0, 1, 1, 2, 2, 3, 3, 0,
        4, 5, 5, 6, 6, 7, 7, 4,
        0, 4, 1, 5, 2, 6, 3, 7
    };
    

    initializeBuffers();
}

Renderer::Renderer() {}


void Renderer::initializeBuffers() {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create and bind vertex buffer
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, lines.size() * sizeof(unsigned short), &lines[0], GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void Renderer::drawWireframeCube(
    const glm::vec3 &size,
    const glm::mat4 &viewMatrix,
    const glm::mat4 &projectionMatrix
) const {
    if (!highlightEnabled) return;

    glm::mat4 ModelMatrix = glm::translate(glm::mat4(1.0f), highlight);
    ModelMatrix = glm::scale(ModelMatrix, size);

    glUseProgram(programID);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(10.0f);
    glDisable(GL_DEPTH_TEST);

    // Set uniforms
    glUniformMatrix4fv(glGetUniformLocation(programID, "ModelMatrix"), 1, GL_FALSE, &ModelMatrix[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(programID, "ViewMatrix"), 1, GL_FALSE, &viewMatrix[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(programID, "ProjectionMatrix"), 1, GL_FALSE, &projectionMatrix[0][0]);


    // Bind VAO and draw
    glBindVertexArray(vao);
    glDrawElements(GL_LINES, lines.size(), GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_DEPTH_TEST);
}


void Renderer::setHighlight(const glm::vec3& highlight) {
    highlightEnabled = true;
    this->highlight = highlight;
}

void Renderer::disableHighlight() {
    highlightEnabled = false;
}