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

    // Ray-cylinder intersection
    bool intersects(const ray& r, double& t_hit) const override {
        vector3 d = r.direction;
        vector3 o = r.origin;

        vector3 v = d - axis * (d.dot(axis)); // Perpendicular to axis
        vector3 w = (o - center) - axis * ((o - center).dot(axis));

        double A = v.dot(v);
        double B = 2.0 * v.dot(w);
        double C = w.dot(w) - radius * radius;

        // Solve quadratic equation
        double discriminant = B * B - 4 * A * C;
        if (discriminant < 0) return false; // No intersection

        double t0 = (-B - sqrt(discriminant)) / (2.0 * A);
        double t1 = (-B + sqrt(discriminant)) / (2.0 * A);

        // Find the nearest valid intersection
        double t_cylinder = t0;
        if (t_cylinder < 0) t_cylinder = t1; // Use t1 if t0 is behind
        // if (t_cylinder < 0) return false;   // No valid intersection

        // Check height bounds
        double y = (o + d * t_cylinder - center).dot(axis);
        if (y < 0 || y > height) return false;

        t_hit = t_cylinder;
        return true;
    }
};

#endif
