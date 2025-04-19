#include "HUD.hpp"
#include "Barre.hpp"
#include "Crosshair.hpp"
#include "common/shader.hpp"

HUD::HUD(int windowWidth, int windowHeight)
    : m_windowWidth(windowWidth), m_windowHeight(windowHeight)
{
    m_crosshair = new Crosshair(0.025f);
    m_barre = new Barre(windowWidth, windowHeight);
}

HUD::~HUD() {
    delete m_crosshair;
    delete m_barre;
}

void HUD::render() {
    glDisable(GL_DEPTH_TEST); // Disable depth test for 2D rendering

    m_crosshair->render();
    m_barre->render();

    glEnable(GL_DEPTH_TEST);
}

void HUD::updateWindowSize(int width, int height) {
    m_windowWidth = width;
    m_windowHeight = height;
    m_barre->updateSize(width, height);
}
