#include "Barre.hpp"
#include "common/shader.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <TP/Textures/TextureManager.hpp>


Barre::Barre(int windowWidth, int windowHeight)
    : m_windowWidth(windowWidth), m_windowHeight(windowHeight)
{
    shaderID = LoadShaders("../shader/barre_vertex_shader.glsl", "../shader/barre_fragment_shader.glsl");
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

    float texCoords[] = {
        TEXTUREATLAS_COORD_UNIT_OFFSET_X32, TEXTUREATLAS_COORD_UNIT_OFFSET_X32,
        TEXTUREATLAS_COORD_UNIT_X32 + TEXTUREATLAS_COORD_UNIT_OFFSET_X32, TEXTUREATLAS_COORD_UNIT_OFFSET_X32,
        TEXTUREATLAS_COORD_UNIT_X32 + TEXTUREATLAS_COORD_UNIT_OFFSET_X32, TEXTUREATLAS_COORD_UNIT_X32 + TEXTUREATLAS_COORD_UNIT_OFFSET_X32,
        TEXTUREATLAS_COORD_UNIT_OFFSET_X32, TEXTUREATLAS_COORD_UNIT_X32 + TEXTUREATLAS_COORD_UNIT_OFFSET_X32
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // Upload vertex data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(slotVertices) + sizeof(texCoords), nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(slotVertices), slotVertices);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(slotVertices), sizeof(texCoords), texCoords);

    // Vertex positions
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Texture coordinates
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(sizeof(slotVertices)));
    glEnableVertexAttribArray(1);

    // Upload index data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void Barre::render() {
    glUseProgram(shaderID);

    TextureManager::getInstance().getPBRTexture("blocks")->bindOnlyTexture(shaderID);

    GLuint resUniform = glGetUniformLocation(shaderID, "u_resolution");
    glUniform2f(resUniform, m_windowWidth, m_windowHeight);

    GLuint modelLoc = glGetUniformLocation(shaderID, "u_model");
    GLuint isSelectedUniform = glGetUniformLocation(shaderID, "u_isSelected");
    GLuint uvOffsetUniform = glGetUniformLocation(shaderID, "u_uvOffset");

    glBindVertexArray(VAO);
    for (int i = 0; i < slots.size(); ++i) {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(slots[i][0], slots[i][1], 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
        glUniform1i(isSelectedUniform, i == inventory->getSelectedIndex() ? 1 : 0);

        // Get the texture coordinates for the item in the slot
        const ItemStack& item = inventory->getItems()[i];
        if (item.getItemId() != AIR) {
            std::pair<float, float> texCoords = BlocDatabase::getInstance().getTexCoords(item.getItemId(), 0);
            glUniform2f(uvOffsetUniform, texCoords.first - TEXTUREATLAS_COORD_UNIT_OFFSET_X32, texCoords.second - TEXTUREATLAS_COORD_UNIT_OFFSET_X32);
        } else {
            // Empty slot, set default texture coordinates
            glUniform2f(uvOffsetUniform, -1.0f, -1.0f);
        }

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