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
    cleanupBuffers();
    glDeleteProgram(shaderID);
    slots.clear();
}

void Barre::initSlots() {
    slots.clear();

    const int slotCount = 9;
    const float slotSize = 64.0f;
    const float spacing = 8.0f;

    // Calcul de la largeur totale occupée par les slots et les espaces entre eux
    float totalWidth = slotCount * slotSize + (slotCount - 1) * spacing;

    // Centrer horizontalement
    float startX = (m_windowWidth - totalWidth) / 2.0f;

    // Espace de 20 pixels du bas de la fenêtre a la barre
    float y = m_windowHeight - slotSize - 20.0f;

    for (int i = 0; i < slotCount; ++i) {
        glm::vec2 slot = { startX + i * (slotSize + spacing), y };
        slots.push_back(slot);
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
    glUseProgram(shaderID);
    GLuint resUniform = glGetUniformLocation(shaderID, "u_resolution");
    glUniform2f(resUniform, m_windowWidth, m_windowHeight);

    GLuint modelLoc = glGetUniformLocation(shaderID, "u_model");

    glBindVertexArray(VAO);
    for (const auto& slot : slots) {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(slot[0], slot[1], 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
    glBindVertexArray(0);
}

void Barre::updateSize(int windowWidth, int windowHeight) {
    m_windowWidth = windowWidth;
    m_windowHeight = windowHeight;
    initSlots();
}

void Barre::cleanupBuffers() {
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(shaderID);
}