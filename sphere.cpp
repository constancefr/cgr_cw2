#include "sphere.h"
#include "vector3.h"
#include <cmath>

Sphere::Sphere(Vector3 center, float radius) : center(center), radius(radius) {}

bool Sphere::intersect(const Vector3& rayOrigin, const Vector3& rayDir, float& t1, float& t2) const {
    // Vector from ray origin to sphere center
    Vector3 oc = rayOrigin - center;

    // Compute the coefficients of the quadratic equation
    float a = rayDir.dot(rayDir);  // Dot product of rayDir with itself
    float b = 2.0f * rayDir.dot(oc); // Dot product of rayDir and (rayOrigin - sphere center)
    float c = oc.dot(oc) - radius * radius;  // Dot product of (rayOrigin - sphere center) with itself - r^2

    // Discriminant of the quadratic equation
    float discriminant = b * b - 4 * a * c;

    if (discriminant < 0) {
        return false;  // No intersection
    }

    // Calculate the two intersection points
    t1 = (-b - sqrt(discriminant)) / (2.0f * a);
    t2 = (-b + sqrt(discriminant)) / (2.0f * a);

    return true;  // Intersection found
}
