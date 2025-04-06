#pragma once

#include <glm/glm.hpp>
#include <TP/Scene/BlocTypes.hpp>

class Ray {
public:
    glm::vec3 origin;
    glm::vec3 direction;

    Ray(glm::vec3 origin, glm::vec3 direction) : origin(origin), direction(direction) {}

    ~Ray() {}

    bool rayIntersectsAABB(const Ray &ray, const glm::vec3 &min, const glm::vec3 &max, float maxDistance) {
        float tmin = (min.x - ray.origin.x) / ray.direction.x;
        float tmax = (max.x - ray.origin.x) / ray.direction.x;

        if (tmin > tmax) std::swap(tmin, tmax);

        float tymin = (min.y - ray.origin.y) / ray.direction.y;
        float tymax = (max.y - ray.origin.y) / ray.direction.y;

        if (tymin > tymax) std::swap(tymin, tymax);

        if ((tmin > tymax) || (tymin > tmax))
            return false;

        if (tymin > tmin)
            tmin = tymin;

        if (tymax < tmax)
            tmax = tymax;

        float tzmin = (min.z - ray.origin.z) / ray.direction.z;
        float tzmax = (max.z - ray.origin.z) / ray.direction.z;

        if (tzmin > tzmax) std::swap(tzmin, tzmax);

        if ((tmin > tzmax) || (tzmin > tmax))
            return false;

        //si la distance entre l'origin du rayon et le point intersecté est supérieur à la distance max, on ne renvoie pas vrai
        if (tmin < 0) {
            if (tmax < 0)
                return false;
            else
                return tmax <= maxDistance;
        } else
            return tmin <= maxDistance;
    }

    int rayIntersectsAABBFace(const Ray &ray, const glm::vec3 &min, const glm::vec3 &max, float maxDistance) {
        // Check for intersection with each face of the AABB
        glm::vec3 faces[6] = {
            glm::vec3(1, 0, 0), // Right
            glm::vec3(-1, 0, 0), // Left
            glm::vec3(0, 1, 0), // Top
            glm::vec3(0, -1, 0), // Bottom
            glm::vec3(0, 0, 1), // Front
            glm::vec3(0, 0, -1) // Back
        };

        for (int i = 0; i < 6; ++i) {
            if (rayIntersectsAABB(ray, min + faces[i], max + faces[i], maxDistance)) {
                switch (i) {
                    case 0: return BLOC_RIGHT;
                    case 1: return BLOC_LEFT;
                    case 2: return BLOC_TOP;
                    case 3: return BLOC_BOTTOM;
                    case 4: return BLOC_FRONT;
                    case 5: return BLOC_BACK;
                }
            }
        }

        return -1; // No intersection with any face
    }

};