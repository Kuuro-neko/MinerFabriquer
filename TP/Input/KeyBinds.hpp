#pragma once

#include <TP/Input/KeyInput.hpp>
#include <vector>
#include <algorithm>

class Keybinds {
private:
Keybinds() = default;

    Keybinds(const Keybinds&) = delete;
    Keybinds& operator=(const Keybinds&) = delete;

    // Rebindable keys -> private
    int toggleDebug = GLFW_KEY_F3;
    int escape = GLFW_KEY_ESCAPE;

public:
    // Singleton instance accessor
    static Keybinds& getInstance() {
        static Keybinds instance;
        return instance;
    }

    // Rebindable keys -> public
    std::vector<int> forward = {GLFW_KEY_W};
    std::vector<int> backward = {GLFW_KEY_S};
    std::vector<int> left = {GLFW_KEY_A};
    std::vector<int> right = {GLFW_KEY_D};
    std::vector<int> jump = {GLFW_KEY_SPACE};
    std::vector<int> sneak = {GLFW_KEY_LEFT_CONTROL};
    std::vector<int> sprint = {GLFW_KEY_LEFT_SHIFT};
    std::vector<int> openInventory = {GLFW_KEY_E};
    std::vector<int> breakBlock = {GLFW_MOUSE_BUTTON_LEFT};
    std::vector<int> placeBlock = {GLFW_MOUSE_BUTTON_RIGHT};
    std::vector<int> selectBlock = {GLFW_MOUSE_BUTTON_MIDDLE};
    std::vector<int> toggleHUD = {GLFW_KEY_F1};
    std::vector<int> takeScreenshot = {GLFW_KEY_F2};

    
    std::vector<int> toggleBoudingBoxes = {toggleDebug, GLFW_KEY_B};
    std::vector<int> toggleWireframe = {toggleDebug, GLFW_KEY_W};
    std::vector<int> toggleChunkBorders = {toggleDebug, GLFW_KEY_G};
    std::vector<int> toggleSpectator = {toggleDebug, GLFW_KEY_N};
    std::vector<int> reloadChunkMeshes = {toggleDebug, GLFW_KEY_A};

    std::vector<int> togglePerpective = {GLFW_KEY_F5};
    std::vector<int> toggleFullscreen = {GLFW_KEY_F11};


public:
    int getToggleDebug() const { return toggleDebug; }
    int getEscape() const { return escape; }
    std::vector<int> getKeysToMonitorForCharacter();
    std::vector<int> getKeysToMonitorForMenu();
};