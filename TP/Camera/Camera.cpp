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
	if (glfwGetKey(_window, GLFW_KEY_R) == GLFW_PRESS) {
		reset();
		return;
	}

	// Handle control mode changes when W is pressed
	if (glfwGetKey(_window, GLFW_KEY_Z) == GLFW_PRESS) {
		if (!m_zPressed) {
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
		m_zPressed = true;
    }
	if (glfwGetKey(_window, GLFW_KEY_Z) == GLFW_RELEASE) {
        m_zPressed = false;
    }

	// Attach of detach camera from target when T is pressed
	if (glfwGetKey(_window, GLFW_KEY_C) == GLFW_PRESS) {
		if (!m_tPressed) {
			m_attached = !m_attached;
		}
		m_tPressed = true;
    }
	if (glfwGetKey(_window, GLFW_KEY_C) == GLFW_RELEASE) {
        m_tPressed = false;
    }

	if (m_attached) {
		// Camera mode 1 (arrow keys controlled camera) while attached
		if (m_mode == 1) {
			if (glfwGetKey(_window, GLFW_KEY_A) == GLFW_PRESS) {
				m_eulerAngle.y += M_PI / 180 * m_rotation_speed * 10 * invertX;
			}
			if (glfwGetKey(_window, GLFW_KEY_D) == GLFW_PRESS) {
				m_eulerAngle.y -= M_PI / 180 * m_rotation_speed * 10 * invertX;
			}
			if (glfwGetKey(_window, GLFW_KEY_W) == GLFW_PRESS) {
				m_eulerAngle.x = Camera_Helper::clamp(m_eulerAngle.x - M_PI / 180 * m_rotation_speed * 10 * invertY, -M_PI_2, M_PI_2);
			}
			if (glfwGetKey(_window, GLFW_KEY_S) == GLFW_PRESS) {
				m_eulerAngle.x = Camera_Helper::clamp(m_eulerAngle.x + M_PI / 180 * m_rotation_speed * 10 * invertY, -M_PI_2, M_PI_2);
			}
			if (glfwGetKey(_window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
				m_distance = Camera_Helper::clamp(m_distance + m_distance_speed * _deltaTime, 1.f, 50.f);
			}
			if (glfwGetKey(_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
				m_distance = Camera_Helper::clamp(m_distance - m_distance_speed * _deltaTime, 1.f, 50.f);
			}
		// Camera mode 0 (mouse controlled camera) while attached
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
				m_eulerAngle.x = Camera_Helper::clamp(Camera_Helper::clipAnglePI(m_eulerAngle.x - offsetY * M_PI / 180 * m_rotation_speed * invertY), -M_PI_2, M_PI_2);
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

		if (glfwGetKey(_window, GLFW_KEY_W) == GLFW_PRESS) {
			m_position += normalize(m_rotation * VEC_FRONT) * m_translation_speed * _deltaTime;
		}
		if (glfwGetKey(_window, GLFW_KEY_S) == GLFW_PRESS) {
			m_position -= normalize(m_rotation * VEC_FRONT) * m_translation_speed * _deltaTime;
		}
		if (glfwGetKey(_window, GLFW_KEY_A) == GLFW_PRESS) {
			m_position += normalize(m_rotation * VEC_RIGHT) * m_translation_speed * _deltaTime;
		}
		if (glfwGetKey(_window, GLFW_KEY_D) == GLFW_PRESS) {
			m_position -= normalize(m_rotation * VEC_RIGHT) * m_translation_speed * _deltaTime;
		}
		if (glfwGetKey(_window, GLFW_KEY_SPACE) == GLFW_PRESS) {
			m_position += normalize(m_rotation * VEC_UP) * m_translation_speed * _deltaTime;
		}
		if (glfwGetKey(_window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
			m_position -= normalize(m_rotation * VEC_UP) * m_translation_speed * _deltaTime;
		}
		if (offsetX != 0) {
			m_eulerAngle.y = Camera_Helper::clipAnglePI(m_eulerAngle.y - offsetX * M_PI / 180 * m_rotation_speed * invertX);
		}
		if (offsetY != 0) {
			m_eulerAngle.x = Camera_Helper::clamp(Camera_Helper::clipAnglePI(m_eulerAngle.x - offsetY * M_PI / 180 * m_rotation_speed * invertY), -M_PI_2, M_PI_2);
		}

	// Camera mode 1 (arrow keys controlled camera) while not attached
	} else if (m_mode == 1) {
		glm::vec3 forward = normalize(m_rotation * VEC_FRONT);
		forward.y = 0;
		forward = normalize(forward);
		if (glfwGetKey(_window, GLFW_KEY_W) == GLFW_PRESS) {
			m_position += forward * m_translation_speed * _deltaTime;
		}
		if (glfwGetKey(_window, GLFW_KEY_S) == GLFW_PRESS) {
			m_position -= forward * m_translation_speed * _deltaTime;
		}
		if (glfwGetKey(_window, GLFW_KEY_A) == GLFW_PRESS) {
			m_position += normalize(m_rotation * VEC_RIGHT) * m_translation_speed * _deltaTime;
		}
		if (glfwGetKey(_window, GLFW_KEY_D) == GLFW_PRESS) {
			m_position -= normalize(m_rotation * VEC_RIGHT) * m_translation_speed * _deltaTime;
		}
		if (glfwGetKey(_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
			m_position -= normalize(m_rotation * VEC_UP) * m_translation_speed * _deltaTime;
		}
		if (glfwGetKey(_window, GLFW_KEY_SPACE) == GLFW_PRESS) {
			m_position += normalize(m_rotation * VEC_UP) * m_translation_speed * _deltaTime;
		}
		if (glfwGetKey(_window, GLFW_KEY_LEFT) == GLFW_PRESS) {
			m_eulerAngle.y += M_PI / 180 * m_rotation_speed * 10 * invertX;
		}
		if (glfwGetKey(_window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			m_eulerAngle.y -= M_PI / 180 * m_rotation_speed * 10 * invertX;
		}
		if (glfwGetKey(_window, GLFW_KEY_UP) == GLFW_PRESS) {
			m_eulerAngle.x = Camera_Helper::clamp(m_eulerAngle.x - M_PI / 180 * m_rotation_speed * 10 * invertY, -M_PI_2, M_PI_2);
		}
		if (glfwGetKey(_window, GLFW_KEY_DOWN) == GLFW_PRESS) {
			m_eulerAngle.x = Camera_Helper::clamp(m_eulerAngle.x + M_PI / 180 * m_rotation_speed * 10 * invertY, -M_PI_2, M_PI_2);
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
	Camera_Helper::computeFinalView(m_projectionMatrix, m_viewMatrix, m_position, m_rotation, m_fovDegree);
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