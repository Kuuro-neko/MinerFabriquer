#include <TP/Scene/Clouds.hpp>
#include "Clouds.hpp"
#include <Defines.hpp>


Clouds::Clouds(Texture &cloudTex, float speed, GLuint programID)
{
    std::cout << "Before GL state: " << glGetError() << std::endl;
    // Set the texture binding index to the next free binding index
    texture = &cloudTex;
    texture->setSamplerName("cloudTexture");

    // Set the program ID and scroll speed
    programId = programID;
    scrollSpeed = speed;

    mesh = MeshObject();

    mesh.vertices = {
        glm::vec3(-1.0f, 0.0f, -1.0f),
        glm::vec3(1.0f, 0.0f, -1.0f),
        glm::vec3(-1.0f, 0.0f, 1.0f),
        glm::vec3(1.0f, 0.0f, 1.0f),
    };

    mesh.triangles = {
        0, 1, 2,
        1, 3, 2,
    };

    mesh.uvs = {
        glm::vec2(0.0f, 0.1f),
        glm::vec2(0.1f, 0.1f),
        glm::vec2(0.0f, 0.0f),
        glm::vec2(0.1f, 0.0f),
        // glm::vec2(0.0f, 0.0f),
        // glm::vec2(1.0f, 0.0f),
        // glm::vec2(0.0f, 1.0f),
        // glm::vec2(1.0f, 1.0f),
    };

    mesh.normals = {
        glm::vec3(0.0f, -1.0f, 0.0f),
        glm::vec3(0.0f, -1.0f, 0.0f),
        glm::vec3(0.0f, -1.0f, 0.0f),
        glm::vec3(0.0f, -1.0f, 0.0f),
    };

    mesh.initializeBuffers();

    transform = Transform(
        glm::vec3(0.0f, height, 0.0f),
        DEFAULT_ROTATION,
        RENDERER_DISTANCE*width
    );
    std::cout << "After GL state: " << glGetError() << std::endl;
}

void Clouds::draw(float time, Character &character)
{
    glm::vec3 pos = character.getWorldPosition();
    transform.m_translation = glm::vec3(pos.x, height, pos.z);
    ModelMatrix = glm::mat4(1.0f);
    ModelMatrix = glm::translate(ModelMatrix, transform.m_translation);
    ModelMatrix = glm::scale(ModelMatrix, glm::vec3(transform.m_scale));

    glUseProgram(programId);

    GLuint modelMatrixId = glGetUniformLocation(programId, "ModelMatrix");
    glUniformMatrix4fv(modelMatrixId, 1, false, &ModelMatrix[0][0]);

    glUniform1f(glGetUniformLocation(programId, "scrollSpeed"), scrollSpeed);

    glUniform1f(glGetUniformLocation(programId, "time"), time);

    texture->bind(programId);

    //glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);

    mesh.draw(programId);

    //glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);

}
