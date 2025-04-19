#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "TP/GUI/Crosshair.hpp"
#include "TP/GUI/Barre.hpp"

class HUD {
public:
    HUD(int windowWidth, int windowHeight);
    ~HUD();

    void render();
    void updateWindowSize(int width, int height);

private:
    int m_windowWidth;
    int m_windowHeight;

    Crosshair* m_crosshair;
    Barre* m_barre;
};
