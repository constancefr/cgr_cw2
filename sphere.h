#ifndef SPHERE_H
#define SPHERE_H

#include "shape.h"
#include "ray.h"
#include "vector3.h"
#include <cmath>

class Sphere : public Shape {
public:
    vector3 center;
    double radius;

    Sphere(const vector3& c, double r, const Material& m) 
        : Shape(m), center(c), radius(r) {}

    // Ray-sphere intersection
    bool intersects(const ray& r, double& t_hit) const override {
        vector3 oc = r.origin - center;
        double a = r.direction.dot(r.direction);
        double b = 2.0 * oc.dot(r.direction);
        double c = oc.dot(oc) - radius * radius;

        double discriminant = b * b - 4.0 * a * c;
        if (discriminant < 0) return false;  // No intersection
        
        double t0 = (-b - sqrt(discriminant)) / (2.0 * a);
        double t1 = (-b + sqrt(discriminant)) / (2.0 * a);
        
        t_hit = t0 < 0 ? t1 : t0;  // Use t1 if t0 is negative

        return true;
    }

    vector3 get_normal(const vector3& point) const {
        return (point - center).unit();
    }
};

#endif