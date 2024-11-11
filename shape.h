#ifndef SHAPE_H
#define SHAPE_H

#include "vector3.h"
#include "ray.h"
#include <memory>

// Abstract Shape class
class Shape {
public:
    virtual bool intersects(const ray& r, double& t_hit) const = 0; // Returns true if the ray intersects
    virtual ~Shape() {}
};

// Sphere class
class Sphere : public Shape {
public:
    vector3 center;
    double radius;

    Sphere(const vector3& c, double r) : center(c), radius(r) {}

    // Ray-sphere intersection
    bool intersects(const ray& r, double& t_hit) const override {
        vector3 oc = r.origin - center;
        double a = r.direction.dot(r.direction);
        double b = 2.0 * oc.dot(r.direction);
        double c = oc.dot(oc) - radius * radius;
        double discriminant = b * b - 4 * a * c;

        /* Debug:
        if (discriminant >= 0) {
            std::cout << "Intersection!!! at t=" << t_hit << "\n";
            std::cout << "Ray: Origin=" << r.origin << ", Direction=" << r.direction << "\n";
            std::cout << "Sphere: Center=" << center << ", Radius=" << radius << "\n";
            std::cout << "Discriminant=" << discriminant << "\n";
        }
        */

        if (discriminant < 0) return false; // No intersection

        // Calculate the two roots
        double sqrt_discriminant = std::sqrt(discriminant);
        double t1 = (-b - sqrt_discriminant) / (2.0 * a);
        double t2 = (-b + sqrt_discriminant) / (2.0 * a);

        // std::cout << "t1 = " << t1 << ", t2 = " << t2 << "\n";
        if (t1 < t2) {
            t_hit = t1;
            // std::cout << "t_hit = " << t_hit << "\n";
        } else {
            t_hit = t2;
            // std::cout << "t_hit = " << t_hit << "\n";
        }

        return true; // Intersection found
    }
};

#endif
