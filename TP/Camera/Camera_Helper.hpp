#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/norm.hpp>


enum InterPolationType
{
	LINEAR, COS, EXP, LOG, SQUARE, SQRT, CUBIC, SQRT3
};

static glm::vec3 VEC_ZERO{ 0.f,0.f,0.f };
static glm::vec3 VEC_UP{ 0.f,1.f,0.f };
static glm::vec3 VEC_FRONT{ 0.f,0.f,1.f };
static glm::vec3 VEC_RIGHT{ 1.f,0.f,0.f };

class Camera_Helper
{
public: 
	static glm::vec3 quatToEuler(glm::quat _quat);
	static void computeFinalView(glm::mat4& _outProjectionMatrix, glm::mat4& _outviewMatrix, glm::vec3& _position, glm::quat _rotation, float _fovDegree);
	static glm::vec3 projectVectorOnPlan(glm::vec3 _vector, glm::vec3 _normal);
	static double clipAnglePI(double _angle);
	static double clamp(double _value, double _min, double _max);
	static double interpolation(double _ratio, InterPolationType _type);
};