// Renderer.hpp
#pragma once

#include <glm/glm.hpp>
#include "GL/glew.h"

class Renderer {

private:
    inline GLuint getProgramID() {
        return programID;
    }

    GLuint programID;

public:

    Renderer (GLuint programID) : programID(programID) {}

    Renderer () {}

    /**
     * @brief Draw a wireframe cube
     *
     * @param position Position of the cube
     * @param size Size of the cube
     * @param color Color of the cube
     */
    void drawWireframeCube(const glm::vec3& position, const glm::vec3& size, const glm::vec3& color) const;


};