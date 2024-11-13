#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "shape.h"
#include "ray.h"
#include "vector3.h"

class Triangle : public Shape {
public:
    vector3 v0, v1, v2;

    Triangle(const vector3& vertex0, const vector3& vertex1, const vector3& vertex2)
        : v0(vertex0), v1(vertex1), v2(vertex2) {}

    // Möller–Trumbore ray-triangle intersection algorithm
    bool intersects(const ray& r, double& t_hit) const override {
        const vector3 edge1 = v1 - v0;
        const vector3 edge2 = v2 - v0;
        const vector3 h = r.direction.cross(edge2);
        const double a = edge1.dot(h);

        if (a > -1e-8 && a < 1e-8) { // Ray is parallel to the triangle
            return false;
        }

        const double f = 1.0 / a;
        const vector3 s = r.origin - v0;
        const double u = f * s.dot(h);

        if (u < 0.0 || u > 1.0) {
            return false; // Intersection is outside the triangle
        }

        const vector3 q = s.cross(edge1);
        const double v = f * r.direction.dot(q);

        if (v < 0.0 || u + v > 1.0) {
            return false; // Intersection is outside the triangle
        }

        // At this point, we have a valid intersection
        t_hit = f * edge2.dot(q);
        // return t_hit > 1e-8; // Ray intersects the triangle
        return true;
    }
};

#endif
