#ifndef CYLINDER_H
#define CYLINDER_H

#include "vector3.h"
#include "ray.h"
#include "shape.h"
#include <cmath>

class Cylinder : public Shape {
public:
    vector3 center;      // Base center
    vector3 axis;        // Unit vector along axis
    double radius;       // Radius
    double height;       // Height (if finite)

    Cylinder(const vector3& c, const vector3& a, double r, double h) 
        : center(c), axis(a.unit()), radius(r), height(h) {}

    bool intersects(const ray& r, double& t_hit) const override {
        vector3 d = r.direction;
        vector3 o = r.origin;

        vector3 v = d - axis * (d.dot(axis)); // Perpendicular to axis
        vector3 w = (o - center) - axis * ((o - center).dot(axis));

        double A = v.dot(v);
        double B = 2.0 * v.dot(w);
        double C = w.dot(w) - radius * radius;

        double t_cylinder = -1.0, t_bottom = -1.0, t_top = -1.0;

        // 1. Cylinder body intersection test
        double discriminant = B * B - 4 * A * C;
        if (discriminant >= 0) {
            double t0 = (-B - sqrt(discriminant)) / (2.0 * A);
            double t1 = (-B + sqrt(discriminant)) / (2.0 * A);

            // Find the nearest valid intersection
            t_cylinder = (t0 >= 0) ? t0 : t1;
            if (t_cylinder >= 0) {
                double y = (o + d * t_cylinder - center).dot(axis);
                if (y < 0 || y > height) t_cylinder = -1.0; // Outside height bounds
            }
        }

        // 2. Bottom cap intersection
        if (std::fabs(d.dot(axis)) > 1e-6) { // Avoid division by zero
            t_bottom = (center - o).dot(axis) / d.dot(axis);
            if (t_bottom >= 0) {
                vector3 p_bottom = o + d * t_bottom;
                if ((p_bottom - center).dot(p_bottom - center) > radius * radius) t_bottom = -1.0;
            }
        }

        // 3. Top cap intersection
        vector3 top_center = center + axis * height;
        if (std::fabs(d.dot(axis)) > 1e-6) { // Avoid division by zero
            t_top = (top_center - o).dot(axis) / d.dot(axis);
            if (t_top >= 0) {
                vector3 p_top = o + d * t_top;
                if ((p_top - top_center).dot(p_top - top_center) > radius * radius) t_top = -1.0;
            }
        }

        // 4. Find the closest valid intersection
        t_hit = -1.0;
        if (t_cylinder >= 0) t_hit = t_cylinder;
        if (t_bottom >= 0 && (t_hit < 0 || t_bottom < t_hit)) t_hit = t_bottom;
        if (t_top >= 0 && (t_hit < 0 || t_top < t_hit)) t_hit = t_top;

        // 5. Return true if a valid intersection is found
        return t_hit >= 0;
    }

};

#endif