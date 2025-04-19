#include "Barre.hpp"
#include "common/shader.hpp"
#include <glm/gtc/matrix_transform.hpp>


Barre::Barre(int windowWidth, int windowHeight)
    : m_windowWidth(windowWidth), m_windowHeight(windowHeight)
{
    shaderID = LoadShaders("barre_vertex_shader.glsl", "barre_fragment_shader.glsl");
    initSlots();
    initBuffers();
}

Barre::~Barre() {
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(shaderID);
}

void Barre::initSlots() {
    slots.clear();
    float totalWidth = 9 * 64 + 8 * 8;
    float startX = (m_windowWidth - totalWidth) / 2.0f;
    float y = 20.0f;

    for (int i = 0; i < 9; ++i) {
        slots.push_back({ startX + i * (64 + 8), y });
    }
}

void Barre::initBuffers() {
    float slotVertices[] = {
        0.0f, 0.0f,
        64.0f, 0.0f,
        64.0f, 64.0f,
        0.0f, 64.0f
    };

    unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(slotVertices), slotVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

void Barre::render() {
glDisable(GL_DEPTH_TEST);

    glUseProgram(shaderID);
    GLuint resUniform = glGetUniformLocation(shaderID, "u_resolution");
    glUniform2f(resUniform, m_windowWidth, m_windowHeight);

    GLuint modelLoc = glGetUniformLocation(shaderID, "u_model");

    glBindVertexArray(VAO);
    for (const auto& slot : slots) {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(slot.x, slot.y, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
    glBindVertexArray(0);
glEnable(GL_DEPTH_TEST);
}

void Barre::updateSize(int windowWidth, int windowHeight) {
    m_windowWidth = windowWidth;
    m_windowHeight = windowHeight;
    initSlots();
}
