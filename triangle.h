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

/*
    // Calculate UV coordinates using barycentric interpolation
    virtual std::pair<double, double> get_uv(const vector3& point) const {
        // Compute barycentric coordinates
        vector3 v0v1 = v1 - v0;
        vector3 v0v2 = v2 - v0;
        vector3 v0p = point - v0;

        double d00 = v0v1.dot(v0v1);
        double d01 = v0v1.dot(v0v2);
        double d11 = v0v2.dot(v0v2);
        double d20 = v0p.dot(v0v1);
        double d21 = v0p.dot(v0v2);

        double denom = d00 * d11 - d01 * d01;
        double v = (d11 * d20 - d01 * d21) / denom;
        double w = (d00 * d21 - d01 * d20) / denom;
        double u = 1.0 - v - w;

        // Interpolate UV coordinates using barycentric coordinates
        // Planar mapping: assume (0, 0) at v0, (1, 0) at v1, (0, 1) at v2 for simplicity
        double u_final = u * 0.0 + v * 1.0 + w * 0.0; // Example: (0,0), (1,0), (0,1) mapping
        double v_final = u * 0.0 + v * 0.0 + w * 1.0;

        return {u_final, v_final}; // Return UV coordinates
    }
*/

};

#endif
