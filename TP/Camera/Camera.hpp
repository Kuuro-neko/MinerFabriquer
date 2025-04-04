#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/norm.hpp>
#include <TP/Camera/Camera_Helper.hpp>


#define DEFAULT_FOV 45.0f
#define DEFAULT_POSITION glm::vec3(0.0f, 0.0f, 0.0f)
#define DEFAULT_EULER_ANGLE glm::vec3(0.537289f, -1.93024f, 0.0f)
#define CAMERA_POSITION_RELATIVE_TO_PLAYER glm::vec3(0.f, 1.5f, 0.f)

#define DEFAULT_TRANSLATION_SPEED 7.5f
#define DEFAULT_ROTATION_SPEED 0.1f
#define DEFAULT_DISTANCE_SPEED 5.0f

#define DEFAULT_ATTACHED false
#define DEFAULT_MODE 1 // 0 for free camera, 1 for third person camera

class Camera
{
public: 

	void reset();
	void init();
	void update(float _deltaTime, GLFWwindow* _window);
	void updateFreeInput(float _deltaTime, GLFWwindow* _window);

	void setTarget(glm::vec3 _target);
	void updateTarget(glm::vec3 _target);

	glm::vec3 getPosition() const {return m_position;}
	void setPosition(glm::vec3 _position) {
		m_newPosition = _position;
		m_needPositionUpdate = true;
	}
	glm::quat getRotation() const {return m_rotation;}

	//View
	glm::mat4 m_viewMatrix;
	glm::mat4 m_projectionMatrix;

	glm::vec3	m_eulerAngle{ DEFAULT_EULER_ANGLE };
private:
	//Camera parameters 
	float		m_fovDegree{ DEFAULT_FOV };
	glm::vec3	m_position{ DEFAULT_POSITION };
	glm::quat	m_rotation{};
	float		m_translation_speed = DEFAULT_TRANSLATION_SPEED;
	float 		m_distance_speed = DEFAULT_DISTANCE_SPEED;
	float       m_rotation_speed = DEFAULT_ROTATION_SPEED;
	

	bool m_invertY = false;
	bool m_invertX = false;

	//Interface option
	bool m_showImguiDemo{ true };

	//Camera mode
	int m_mode = DEFAULT_MODE;
	bool m_zPressed = false;
	double m_prevMouseX = 0.0, m_prevMouseY = 0.0;

	//Reset animation
	bool m_resetting = false;
	float m_resetTime = 0.;
	float m_resetDuration = 1.;
	//  Initial values of reset animation
	float m_initialFov;
	glm::vec3 m_initialPosition;
	glm::vec3 m_initialEulerAngle;
	//  Target values of reset animation (default values of the camera)
	float m_targetFov = DEFAULT_FOV;
	glm::vec3 m_targetPosition = DEFAULT_POSITION;
	glm::vec3 m_targetEulerAngle = DEFAULT_EULER_ANGLE;

	//Camera Third
	glm::vec3 m_targetDeltaPos;
	glm::vec3 m_targetPrev;
	bool m_attached = DEFAULT_ATTACHED;
	bool m_tPressed = false;
	float m_distance = 10.0f;

	glm::vec3 m_newPosition;
	bool m_needPositionUpdate = false;
};