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
        float t[6];

        glm::vec3 dirfrac = glm::vec3(1.f) / ray.direction;

        t[0] = (min.x - ray.origin.x) * dirfrac.x;
        t[1] = (max.x - ray.origin.x) * dirfrac.x;
        t[2] = (min.y - ray.origin.y) * dirfrac.y;
        t[3] = (max.y - ray.origin.y) * dirfrac.y;
        t[4] = (min.z - ray.origin.z) * dirfrac.z;
        t[5] = (max.z - ray.origin.z) * dirfrac.z;

        float tmin = glm::max(glm::max(glm::min(t[0], t[1]), glm::min(t[2], t[3])), glm::min(t[4], t[5]));
        float tmax = glm::min(glm::min(glm::max(t[0], t[1]), glm::max(t[2], t[3])), glm::max(t[4], t[5]));
        if (tmax < 0) return -1;
        if (tmin > tmax) return -1;
        if (tmin < 0) {
            if (tmax < 0)
                return -1;
            else {
                if (tmax <= maxDistance) {
                    if (t[0] == tmin) return FACE_EAST;
                    if (t[1] == tmin) return FACE_WEST;
                    if (t[2] == tmin) return FACE_TOP;
                    if (t[3] == tmin) return FACE_BOTTOM;
                    if (t[4] == tmin) return FACE_NORTH;
                    if (t[5] == tmin) return FACE_SOUTH;
                } else {
                    return -1;
                }
            }
        } else {
            if (tmin <= maxDistance) {
                if (t[0] == tmin) return FACE_EAST;
                if (t[1] == tmin) return FACE_WEST;
                if (t[2] == tmin) return FACE_TOP;
                if (t[3] == tmin) return FACE_BOTTOM;
                if (t[4] == tmin) return FACE_NORTH;
                if (t[5] == tmin) return FACE_SOUTH;
            } else
                return -1;
        }
            
    }

};