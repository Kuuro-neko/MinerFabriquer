#pragma once

#include <GLFW/glfw3.h>
#include <map>
#include <vector>

#define KEY_PRESSED    1
#define KEY_HELD       2
#define KEY_RELEASED   3
#define KEY_UNPRESSED  4
#define KEY_UNDEFINED  5

class KeyInput
{
    // Main KeyInput functionality
public:
    // Takes a list of which keys to keep state for
    KeyInput(std::vector<int> keysToMonitor);
    ~KeyInput();
    // If this KeyInput is enabled and the given key is monitored,
    // returns pressed state.  Else returns false.
    bool isKeyHeld(int key);
    bool isKeyReleased(int key);
    bool isKeyPressed(int key);

    bool isKeybindHeld(const std::vector<int>& keybind);
    bool isKeybindPressed(const std::vector<int>& keybind);
    bool isKeybindReleased(const std::vector<int>& keybind);
    
    int getKeyStatus(int key);
    // See _isEnabled for details
    bool getIsEnabled() { return _isEnabled; }
    void setIsEnabled(bool value) { _isEnabled = value; }
    // Updates the state of all keys 
    void update();

private:
    // Used internally to update key states.  Called by the GLFW callback.
    void setKeyStatus(int key, int action);
    // Map from monitored keyes to their pressed states
    std::map<int, char> _keys;
    // If disabled, KeyInput.getIsKeyDown always returns false
    bool _isEnabled;

    // Workaround for C++ class using a c-style-callback
public:
    // Must be called before any KeyInput instances will work
    static void setupKeyInputs(GLFWwindow &window);

private:
    // The GLFW callback for key events.  Sends events to all KeyInput instances
    static void keysCallback(
        GLFWwindow *window, int key, int scancode, int action, int mods);
    
    static void mouseCallback(GLFWwindow *window, int button, int action, int mods);
    // Keep a list of all KeyInput instances and notify them all of key events
    static std::vector<KeyInput *> _instances;
};