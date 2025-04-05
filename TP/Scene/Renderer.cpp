// Renderer.cpp
#include "Renderer.hpp"
#include <GL/glew.h>
#include <iostream>

void Renderer::drawWireframeCube(const glm::vec3 &position, const glm::vec3 &size, const glm::vec3 &color) const {
    //glUseProgram(programID);
    std::cout << "programID: " << programID << std::endl;

    glPushMatrix();
    glTranslatef(position.x, position.y, position.z);
    glColor3f(color.r, color.g, color.b);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBegin(GL_LINES);

    // Front face
    glVertex3f(0, 0, 0);
    glVertex3f(size.x, 0, 0);
    glVertex3f(size.x, 0, 0);
    glVertex3f(size.x, size.y, 0);
    glVertex3f(size.x, size.y, 0);
    glVertex3f(0, size.y, 0);
    glVertex3f(0, size.y, 0);
    glVertex3f(0, 0, 0);

    // Back face
    glVertex3f(0, 0, size.z);
    glVertex3f(size.x, 0, size.z);
    glVertex3f(size.x, 0, size.z);
    glVertex3f(size.x, size.y, size.z);
    glVertex3f(size.x, size.y, size.z);
    glVertex3f(0, size.y, size.z);
    glVertex3f(0, size.y, size.z);
    glVertex3f(0, 0, size.z);

    // Connecting edges
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, size.z);
    glVertex3f(size.x, 0, 0);
    glVertex3f(size.x, 0, size.z);
    glVertex3f(size.x, size.y, 0);
    glVertex3f(size.x, size.y, size.z);
    glVertex3f(0, size.y, 0);
    glVertex3f(0, size.y, size.z);

    glEnd();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glPopMatrix();
}