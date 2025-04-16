#include "KeyInput.hpp"
#include <algorithm>
#include <iostream>

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

bool KeyInput::isKeyDown(int key)
{
    bool result = false;
    if (_isEnabled)
    {
        std::map<int, char>::iterator it = _keys.find(key);
        if (it != _keys.end())
        {
            result = (_keys[key] == KEY_PRESSED || _keys[key] == KEY_HELD);
        }
    }
    return result;
}

bool KeyInput::isKeyReleased(int key)
{
    return (_isEnabled && _keys[key] == KEY_RELEASED);
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
                _keys[key] = KEY_PRESSED;
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
    glfwSetKeyCallback(&window, KeyInput::callback);
}

void KeyInput::callback(GLFWwindow *window, int key, int scancode, int action, int mods)

{
    // Send key event to all KeyInput instances
    for (KeyInput *keyInput : _instances)
    {
        keyInput->setKeyStatus(key, action);
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
