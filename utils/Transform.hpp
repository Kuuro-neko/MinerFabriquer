#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define AXIS_X glm::vec3(1.0f, 0.0f, 0.0f)
#define AXIS_Y glm::vec3(0.0f, 1.0f, 0.0f)
#define AXIS_Z glm::vec3(0.0f, 0.0f, 1.0f)

#define DEFAULT_TRANSLATION glm::vec3(0.0f)
#define DEFAULT_ROTATION glm::mat3x3(1.0f)
#define DEFAULT_SCALE 1.0f

class Transform
{
public:
    float m_scale;
    glm::mat3x3 m_rotation;
    glm::vec3 m_translation;
    Transform(glm::vec3 translation = DEFAULT_TRANSLATION, glm::mat3x3 rotation = DEFAULT_ROTATION, float scale = DEFAULT_SCALE)
    : m_scale(scale), m_rotation(rotation), m_translation(translation) {}

    ~Transform() {}

    void rotate(float angle, glm::vec3 axis) {
        m_rotation = glm::mat3x3(glm::rotate(glm::mat4(1.0f), angle, axis));
    }

    void translate(glm::vec3 translation) {
        m_translation += translation;
    }

    void scale(float scale) {
        m_scale *= scale;
    }
};