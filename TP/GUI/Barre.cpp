#include "Barre.hpp"
#include "common/shader.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>


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
    GLuint isSelectedUniform = glGetUniformLocation(shaderID, "u_isSelected");

    glBindVertexArray(VAO);
    for (int i = 0; i < slots.size(); ++i) {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(slots[i][0], slots[i][1], 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
        glUniform1i(isSelectedUniform, i == m_selectedSlot ? 1 : 0);
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

void Barre::setSelectedSlot(int selectedSlot) {
    m_selectedSlot = selectedSlot;
}

int Barre::getSelectedSlot() const {
    return m_selectedSlot;
}

void Barre::nextSelectedSlot(int pos) {
    const int size = static_cast<int>(slots.size());
    if(pos<0){
        m_selectedSlot = (m_selectedSlot - 1 + size) % size;
    }else if (pos>0){
        std::cout << "positif " << std::endl;

        m_selectedSlot = (m_selectedSlot + 1) % size;
    }
    std::cout << "Selected slot: " << m_selectedSlot << std::endl;

}