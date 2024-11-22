#ifndef SPHERE_H
#define SPHERE_H
// #endif
// #ifndef M_PI
#define M_PI 3.14159265358979323846

#include "shape.h"
#include "ray.h"
#include "vector3.h"
#include <cmath>

class Sphere : public Shape {
public:
    vector3 center;
    double radius;
    Material material;

    Sphere(const vector3& c, double r, const Material& m) 
        : center(c), radius(r), Shape(m) {}

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

    std::pair<double, double> get_uv(const vector3& point) const override {
        vector3 p = (point - center).unit();  // Normalize to unit sphere
        double u = 0.5 + atan2(p.z, p.x) / (2 * M_PI); // Angle around Y-axis
        double v = 0.5 - asin(p.y) / M_PI;            // Vertical angle
        return {u, v};
    }

    AABB get_bbox() const override {
        AABB bbox;
        bbox.min = center - vector3(radius, radius, radius);
        bbox.max = center + vector3(radius, radius, radius);
        return bbox;
    }

};

#endif