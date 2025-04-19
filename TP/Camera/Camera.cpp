#include <TP/Camera/Camera.hpp>
#include <TP/Camera/Camera_Helper.hpp>


#include <stdio.h>
#include <iostream>

void Camera::reset()
{
	if (m_resetting || m_attached) return;
	m_initialFov = m_fovDegree;
	m_initialPosition = m_position;
	m_initialEulerAngle = m_eulerAngle;

	m_rotation_speed = DEFAULT_ROTATION_SPEED;
	m_translation_speed = DEFAULT_TRANSLATION_SPEED;
	m_distance_speed = DEFAULT_DISTANCE_SPEED;
	m_attached = DEFAULT_ATTACHED;
	m_resetting = true;
	m_resetTime = 0.;
}

void Camera::init()
{
	m_showImguiDemo = false;
}

void Camera::updateFreeInput(float _deltaTime, GLFWwindow* _window)
{
	if (m_resetting) return;
	int w, h;
	int invertY = m_invertY ? -1 : 1;
	int invertX = m_invertX ? -1 : 1;
	glfwGetWindowSize(_window, &w, &h);

	// On key R, reset the camera
	// if (glfwGetKey(_window, GLFW_KEY_R) == GLFW_PRESS) {
	// 	reset();
	// 	return;
	// }

	// Handle control mode changes when W is pressed
	if (keyInput->isKeybindPressed(keybinds->toggleFocus)) {
		m_mode = (m_mode+1) % 2;
		if (m_mode == 0) {
			glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			m_prevMouseX = w / 2.0;
			m_prevMouseY = h / 2.0;
			glfwSetCursorPos(_window, m_prevMouseX, m_prevMouseY);
		} else {
			glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			m_prevMouseX = w / 2.0;
			m_prevMouseY = h / 2.0;
			glfwSetCursorPos(_window, m_prevMouseX, m_prevMouseY);
		}
    }

	// Attach of detach camera from target when togglePerspective key is pressed
	if (keyInput->isKeybindPressed(keybinds->togglePerspective) || keyInput->isKeybindPressed(keybinds->togglePerspectiveAlternative)) {
		if (m_attached) {
			setPosition(m_targetPrev + m_targetDeltaPos + CAMERA_POSITION_RELATIVE_TO_PLAYER);
		}
		m_attached = !m_attached;
    }

	if (m_attached) {
		if (m_mode == 0) {
			double mouseX, mouseY;
			glfwGetCursorPos(_window, &mouseX, &mouseY);
	
			double offsetX = mouseX - m_prevMouseX;
			double offsetY = m_prevMouseY - mouseY;
	
			m_prevMouseX = w / 2.0;
			m_prevMouseY = h / 2.0;
			glfwSetCursorPos(_window, m_prevMouseX, m_prevMouseY);
			if (offsetX != 0) {
				m_eulerAngle.y = Camera_Helper::clipAnglePI(m_eulerAngle.y - offsetX * M_PI / 180 * m_rotation_speed * invertX);
			}
			if (offsetY != 0) {
				m_eulerAngle.x = Camera_Helper::clamp(Camera_Helper::clipAnglePI(m_eulerAngle.x - offsetY * M_PI / 180 * m_rotation_speed * invertY), -M_PI_2 + 0.1f, M_PI_2 - 0.1f);
			}
		} else {
			if (keyInput->isKeybindHeld(keybinds->keyCameraLeft)) {
				m_eulerAngle.y = Camera_Helper::clipAnglePI(m_eulerAngle.y + m_rotation_speed * M_PI / 180 * m_rotationSpeedKeysCorrection);
			}
			if (keyInput->isKeybindHeld(keybinds->keyCameraRight)) {
				m_eulerAngle.y = Camera_Helper::clipAnglePI(m_eulerAngle.y - m_rotation_speed * M_PI / 180 * m_rotationSpeedKeysCorrection);
			}
			if (keyInput->isKeybindHeld(keybinds->keyCameraUp)) {
				m_eulerAngle.x = Camera_Helper::clamp(Camera_Helper::clipAnglePI(m_eulerAngle.x - m_rotation_speed * M_PI / 180 * m_rotationSpeedKeysCorrection), -M_PI_2 + 0.1f, M_PI_2 - 0.1f);
			}
			if (keyInput->isKeybindHeld(keybinds->keyCameraDown)) {
				m_eulerAngle.x = Camera_Helper::clamp(Camera_Helper::clipAnglePI(m_eulerAngle.x + m_rotation_speed * M_PI / 180 * m_rotationSpeedKeysCorrection), -M_PI_2 + 0.1f, M_PI_2 - 0.1f);
			}
		}
	// Camera mode 0 (mouse controlled camera) while not attached
	} else if (m_mode == 0) {
		double mouseX, mouseY;
		glfwGetCursorPos(_window, &mouseX, &mouseY);

		double offsetX = mouseX - m_prevMouseX;
		double offsetY = m_prevMouseY - mouseY;

		m_prevMouseX = w / 2.0;
		m_prevMouseY = h / 2.0;
		glfwSetCursorPos(_window, m_prevMouseX, m_prevMouseY);

		if (offsetX != 0) {
			m_eulerAngle.y = Camera_Helper::clipAnglePI(m_eulerAngle.y - offsetX * M_PI / 180 * m_rotation_speed * invertX);
		}
		if (offsetY != 0) {
            m_eulerAngle.x = Camera_Helper::clamp(Camera_Helper::clipAnglePI(m_eulerAngle.x - offsetY * M_PI / 180 * m_rotation_speed * invertY), -M_PI_2 + 0.1f, M_PI_2 - 0.1f);
        }
	} else {
		if (keyInput->isKeybindHeld(keybinds->keyCameraLeft)) {
			m_eulerAngle.y = Camera_Helper::clipAnglePI(m_eulerAngle.y + m_rotation_speed * M_PI / 180 * m_rotationSpeedKeysCorrection);
		}
		if (keyInput->isKeybindHeld(keybinds->keyCameraRight)) {
			m_eulerAngle.y = Camera_Helper::clipAnglePI(m_eulerAngle.y - m_rotation_speed * M_PI / 180 * m_rotationSpeedKeysCorrection);
		}
		if (keyInput->isKeybindHeld(keybinds->keyCameraUp)) {
			m_eulerAngle.x = Camera_Helper::clamp(Camera_Helper::clipAnglePI(m_eulerAngle.x - m_rotation_speed * M_PI / 180 * m_rotationSpeedKeysCorrection), -M_PI_2 + 0.1f, M_PI_2 - 0.1f);
		}
		if (keyInput->isKeybindHeld(keybinds->keyCameraDown)) {
			m_eulerAngle.x = Camera_Helper::clamp(Camera_Helper::clipAnglePI(m_eulerAngle.x + m_rotation_speed * M_PI / 180 * m_rotationSpeedKeysCorrection), -M_PI_2 + 0.1f, M_PI_2 - 0.1f);
		}
	}
}


void Camera::update(float _deltaTime, GLFWwindow* _window)
{
	updateFreeInput(_deltaTime, _window);

	if (m_resetting) {
		double ratio = Camera_Helper::interpolation(Camera_Helper::clamp(m_resetTime / m_resetDuration,0.,1.), InterPolationType::SQRT);
		m_fovDegree = ratio * m_targetFov + (1. - ratio) * m_initialFov;
		m_position = glm::vec3(ratio) * m_targetPosition + glm::vec3(1. - ratio) * m_initialPosition;
		m_eulerAngle = glm::vec3(ratio) * m_targetEulerAngle + glm::vec3(1. - ratio) * m_initialEulerAngle;
		m_resetTime += _deltaTime;
		if (m_resetTime > m_resetDuration) {
			m_resetting = false;
			// Fix approximation errors
			m_fovDegree = m_targetFov;
			m_position = m_targetPosition;
			m_eulerAngle = m_targetEulerAngle;
		}
	}

	if (m_attached) {
		//Rotation autour de la target
		m_rotation = glm::quat(m_eulerAngle);
		m_position = m_targetPrev - m_rotation * VEC_FRONT * m_distance;
	} else {
		m_rotation = glm::quat(m_eulerAngle);
	}
	Camera_Helper::computeFinalView(m_projectionMatrix, m_viewMatrix, m_position, m_rotation, m_fovDegree, m_nearPlane, m_farPlane);
}

void Camera::setTarget(glm::vec3 _target)
{
	m_targetPrev = _target;
}


void Camera::updateTarget(glm::vec3 _target)
{
	m_targetDeltaPos = _target - m_targetPrev;
	m_targetPrev = _target;
}