#ifndef TUTORIALS_FRUSTRUM_H
#define TUTORIALS_FRUSTRUM_H

#include <glm/glm.hpp>
#include <TP/Camera/Camera.hpp>

struct Plane {
    // unit vector
    glm::vec3 normal = {0.f, 1.f, 0.f};

    // distance from origin to the nearest point in the plane
    float distance = 0.f;

};


class Frustrum {
public:
    Frustrum(Camera &camera, float aspectRatio, float nearPlane, float farPlane) :
            m_camera(camera),
            m_aspectRatio(aspectRatio),
            m_nearPlane(nearPlane),
            m_farPlane(farPlane) {
        update();
    }

    void update();
    bool isBoundingBoxInFrustum(const glm::vec3& min, const glm::vec3& max) const;

private:
    Camera &m_camera;
    float m_aspectRatio;
    float m_nearPlane;
    float m_farPlane;
    Plane m_planes[6]; // Frustum planes: left, right, top, bottom, near, far
};


#endif //TUTORIALS_FRUSTRUM_H