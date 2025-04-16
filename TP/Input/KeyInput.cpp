#include "KeyInput.hpp"
#include <algorithm>
#include <iostream>
#include <TP/Input/KeyBinds.hpp>

std::vector<KeyInput *> KeyInput::_instances;

KeyInput::KeyInput(std::vector<int> keysToMonitor) : _isEnabled(true)
{
    for (int key : keysToMonitor)
    {
        _keys[key] = KEY_UNPRESSED;
    }
    // Add this instance to the list of instances
    KeyInput::_instances.push_back(this);
}

KeyInput::~KeyInput()
{
    // Remove this instance from the list of instances
    _instances.erase(std::remove(_instances.begin(), _instances.end(), this), _instances.end());
}

bool KeyInput::isKeyHeld(int key)
{
    if (!_isEnabled) return false;

    bool result = false;
    
    std::map<int, char>::iterator it = _keys.find(key);
    if (it != _keys.end())
    {
        result = (_keys[key] == KEY_PRESSED || _keys[key] == KEY_HELD);
    }
    return result;
}

bool KeyInput::isKeybindHeld(const std::vector<int>& keybind)
{
    if (!_isEnabled || keybind.empty()) return false;

    for (int key : keybind)
    {
        if (_keys.find(key) == _keys.end() || _keys[key] != KEY_PRESSED && _keys[key] != KEY_HELD)
        {
            return false;
        }
    }
    return true;

}

bool KeyInput::isKeyPressed(int key)
{
    if (!_isEnabled) return false;
    bool result = false;

    std::map<int, char>::iterator it = _keys.find(key);
    if (it != _keys.end())
    {
        result = (_keys[key] == KEY_PRESSED);
    }
    return result;
}

bool KeyInput::isKeybindPressed(const std::vector<int>& keybind) {
    if (!_isEnabled || keybind.empty()) return false;

    bool anyJustPressed = false;

    for (int key : keybind) {
        auto it = _keys.find(key);
        if (it == _keys.end()) return false; // Key not tracked

        int status = it->second;
        if (status != KEY_PRESSED && status != KEY_HELD) {
            return false; // One key not pressed at all
        }

        if (status == KEY_PRESSED) {
            anyJustPressed = true; // At least one was just pressed
        }
    }

    return anyJustPressed;
}

bool KeyInput::isKeybindReleased(const std::vector<int> &keybind)
{
    if (!_isEnabled || keybind.empty()) return false;

    for (int key : keybind)
    {
        auto it = _keys.find(key);
        if (it == _keys.end()) return false; // Key not tracked

        int status = it->second;
        if (status != KEY_RELEASED) {
            return false; // One key not released
        }
    }
    return true; // All keys released
}

bool KeyInput::isKeyReleased(int key)
{
    if (!_isEnabled) return false;
    bool result = false;
    std::map<int, char>::iterator it = _keys.find(key);
    if (it != _keys.end())
    {
        result = (_keys[key] == KEY_RELEASED);
    }
    return result;
}

int KeyInput::getKeyStatus(int key)
{
    if (_keys.find(key) == _keys.end()) {
        //std::cout << "[KeyInput] Key " << key << " not found in keyStatus map!" << std::endl;
        return KEY_UNDEFINED;
    }
    //std::cout << "[KeyInput] Key " << key << " status: " << (int)_keys[key] << std::endl;
    return (int)_keys[key];
}

void KeyInput::setKeyStatus(int key, int action)
{
    std::map<int, char>::iterator it = _keys.find(key);
    if (it != _keys.end())
    {
        switch (action) {
            case GLFW_PRESS:
                if (_keys[key] == KEY_UNPRESSED)
                {
                    _keys[key] = KEY_PRESSED;
                }
                else
                {
                    _keys[key] = KEY_HELD;
                }
                break;
            case GLFW_REPEAT:
                _keys[key] = KEY_HELD;
                break;
            case GLFW_RELEASE:
                _keys[key] = KEY_RELEASED;
                break;
            default:
                _keys[key] = KEY_UNPRESSED;
                break;
        }
    }
}

void KeyInput::setupKeyInputs(GLFWwindow &window)
{
    glfwSetKeyCallback(&window, KeyInput::keysCallback);
    glfwSetMouseButtonCallback(&window, KeyInput::mouseCallback);
}

void KeyInput::keysCallback(GLFWwindow *window, int key, int scancode, int action, int mods)

{
    // Send key event to all KeyInput instances
    for (KeyInput *keyInput : _instances)
    {
        keyInput->setKeyStatus(key, action);
    }
}

void KeyInput::mouseCallback(GLFWwindow *window, int button, int action, int mods)
{
    // Send mouse event to all KeyInput instances
    for (KeyInput *keyInput : _instances)
    {
        keyInput->setKeyStatus(button, action);
    }
}

void KeyInput::update()
{
    for (auto &pair : _keys)
    {
        char &state = pair.second;
        if (state == KEY_PRESSED)
        {
            state = KEY_HELD;
        }
        else if (state == KEY_RELEASED)
        {
            state = KEY_UNPRESSED;
        }
    }
}
