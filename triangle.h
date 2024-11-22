#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "shape.h"
#include "ray.h"
#include "vector3.h"

class Triangle : public Shape {
public:
    vector3 v0, v1, v2;           // Vertices of the triangle
    vector3 normal;               // Precomputed normal vector
    vector3 uv0, uv1, uv2;        // UV coordinates for each vertex

    // Constructor now accepts UV coordinates for each vertex??
    Triangle(const vector3& v0, const vector3& v1, const vector3& v2, 
            //  const vector3& uv0, const vector3& uv1, const vector3& uv2, 
             const Material& mat)
        : Shape(mat), v0(v0), v1(v1), v2(v2) {
        // Calculate the normal when the triangle is created
        vector3 edge1 = v1 - v0;
        vector3 edge2 = v2 - v0;
        normal = edge1.cross(edge2).unit(); // Cross product of two edges, normalized
    }

    vector3 get_normal(const vector3& point) const {
        return normal; // Precomputed during initialization
    }

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
        return t_hit > 1e-8; // Ray intersects the triangle
    }

    virtual std::pair<double, double> get_uv(const vector3& point) const override {
        double min_x = -1.0; // Surface bounds in the X direction
        double max_x = 1.0;
        double min_z = 0.0;  // Surface bounds in the Z direction
        double max_z = 2.0;

        // Project point onto the X-Z plane and calculate UV
        double u = (point.x - min_x) / (max_x - min_x);
        double v = (point.z - min_z) / (max_z - min_z);

        return {u, v};
    }

    // Get bounding box for the triangle
    AABB get_bbox() const override {
        AABB bbox;
        bbox.min = vector3(
            std::min({v0.x, v1.x, v2.x}),
            std::min({v0.y, v1.y, v2.y}),
            std::min({v0.z, v1.z, v2.z})
        );
        bbox.max = vector3(
            std::max({v0.x, v1.x, v2.x}),
            std::max({v0.y, v1.y, v2.y}),
            std::max({v0.z, v1.z, v2.z})
        );
        return bbox;
    }

};

#endif
