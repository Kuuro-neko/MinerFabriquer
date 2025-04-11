#include "Frustrum.hpp"


void Frustrum::update() {
    // Get the combined view-projection matrix
    glm::mat4 viewProjection = m_camera.getProjectionMatrix() * m_camera.getViewMatrix();

    // Extract the planes from the view-projection matrix
    // Left plane
    m_planes[0].normal = glm::vec3(viewProjection[0][3] + viewProjection[0][0],
                                   viewProjection[1][3] + viewProjection[1][0],
                                   viewProjection[2][3] + viewProjection[2][0]);
    m_planes[0].distance = viewProjection[3][3] + viewProjection[3][0];

    // Right plane
    m_planes[1].normal = glm::vec3(viewProjection[0][3] - viewProjection[0][0],
                                   viewProjection[1][3] - viewProjection[1][0],
                                   viewProjection[2][3] - viewProjection[2][0]);
    m_planes[1].distance = viewProjection[3][3] - viewProjection[3][0];

    // Top plane
    m_planes[2].normal = glm::vec3(viewProjection[0][3] - viewProjection[0][1],
                                   viewProjection[1][3] - viewProjection[1][1],
                                   viewProjection[2][3] - viewProjection[2][1]);
    m_planes[2].distance = viewProjection[3][3] - viewProjection[3][1];

    // Bottom plane
    m_planes[3].normal = glm::vec3(viewProjection[0][3] + viewProjection[0][1],
                                   viewProjection[1][3] + viewProjection[1][1],
                                   viewProjection[2][3] + viewProjection[2][1]);
    m_planes[3].distance = viewProjection[3][3] + viewProjection[3][1];

    // Near plane
    m_planes[4].normal = glm::vec3(viewProjection[0][3] + viewProjection[0][2],
                                   viewProjection[1][3] + viewProjection[1][2],
                                   viewProjection[2][3] + viewProjection[2][2]);
    m_planes[4].distance = viewProjection[3][3] + viewProjection[3][2];

    // Far plane
    m_planes[5].normal = glm::vec3(viewProjection[0][3] - viewProjection[0][2],
                                   viewProjection[1][3] - viewProjection[1][2],
                                   viewProjection[2][3] - viewProjection[2][2]);
    m_planes[5].distance = viewProjection[3][3] - viewProjection[3][2];

    // Normalize the planes
    for (auto &plane : m_planes) {
        float length = glm::length(plane.normal);
        plane.normal /= length;
        plane.distance /= length;
    }
}

bool Frustrum::isBoundingBoxInFrustum(const glm::vec3& min, const glm::vec3& max) const {
    for (const auto& plane : m_planes) {
        // Check if all points of the bounding box are outside the plane
        if (plane.normal.x * (plane.normal.x > 0 ? max.x : min.x) +
            plane.normal.y * (plane.normal.y > 0 ? max.y : min.y) +
            plane.normal.z * (plane.normal.z > 0 ? max.z : min.z) +
            plane.distance < 0) {
            return false; // Bounding box is outside this plane
        }
    }
    return true; // Bounding box is inside or intersects the frustum
}

