#include <TP/Camera/Camera_Helper.hpp>



glm::vec3 Camera_Helper::quatToEuler(glm::quat _quat)
{
	// Opengl quat to euler function give yaw betweel -90 and 90
	// If you want correct pitch and yaw you can use this 
	// Src http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/index.htm
	// But roll will be bewteen -90 and 90 here

	glm::vec3 eulerAngles;

	float test = _quat.x * _quat.y + _quat.z * _quat.w;

	if (test > 0.499f) { // singularity at north pole
		eulerAngles.y = 2.f * atan2(_quat.x, _quat.w);
		eulerAngles.z = M_PI / 2.f;
		eulerAngles.x = 0.f;
		return eulerAngles;
	}
	if (test < -0.499f) { // singularity at south pole
		eulerAngles.y = -2.f * atan2(_quat.x, _quat.w);
		eulerAngles.z = -M_PI / 2.f;
		eulerAngles.x = 0.f;
		return eulerAngles;
	}
	float sqx = _quat.x * _quat.x;
	float sqy = _quat.y * _quat.y;
	float sqz = _quat.z * _quat.z;


	eulerAngles.y = atan2(2.f * _quat.y * _quat.w - 2.f * _quat.x * _quat.z, 1.f - 2.f * sqy - 2.f * sqz);
	eulerAngles.z = asin(2.f * test);
	eulerAngles.x = atan2(2.f * _quat.x * _quat.w - 2.f * _quat.y * _quat.z, 1.f - 2.f * sqx - 2.f * sqz);

	return eulerAngles;
}



void Camera_Helper::computeFinalView(glm::mat4& _outProjectionMatrix, glm::mat4& _outviewMatrix, glm::vec3& _position, glm::quat _rotation, float _fovDegree)
{
	// Projection matrix : FOV, 4:3 ratio, display range : 0.1 unit <-> 100 units
	_outProjectionMatrix = glm::perspective(glm::radians(_fovDegree), 4.0f / 3.0f, 0.1f, 100.0f);

	const glm::vec3 front = normalize(_rotation* VEC_FRONT);
	const glm::vec3 up = normalize(_rotation * VEC_UP);

	// Camera matrix
	_outviewMatrix = glm::lookAt(_position, _position + front, up);
}

glm::vec3 Camera_Helper::projectVectorOnPlan(glm::vec3 _vector, glm::vec3 _normal) {
	return _vector - glm::dot(_vector, _normal) * _normal;
}

double Camera_Helper::clipAnglePI(double _angle) {
	while(_angle < -M_PI) {
		_angle += 2*M_PI;
	}
	while(_angle > M_PI) {
		_angle -= 2*M_PI;
	}
	return _angle;
}

double Camera_Helper::clamp(double _value, double _min, double _max) {
	if(_value < _min) {
		return _min;
	}
	if(_value > _max) {
		return _max;
	}
	return _value;
}

double Camera_Helper::interpolation(double _ratio, InterPolationType _type) {
	switch(_type) {
		case LINEAR:
			return _ratio;
		case COS:
			return 0.5 - 0.5 * cos(_ratio * M_PI);
		case EXP:
			return (exp(_ratio)-1) / (exp(1)-1);
		case LOG:
			return log(_ratio + 1) / log(2);
		case SQUARE:
			return _ratio * _ratio;
		case SQRT:
			return sqrt(_ratio);
		case CUBIC:
			return _ratio * _ratio * _ratio;
		case SQRT3:
			return pow(_ratio, 1. / 3.);
	}
	return 0;
}