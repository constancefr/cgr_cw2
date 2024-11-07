#ifndef SPHERE_H
#define SPHERE_H

#include <cmath>
#include "vector3.h"

class Sphere {
public:
    Sphere(Vector3 center, float radius);

    bool intersect(const Vector3& rayOrigin, const Vector3& rayDir, float& t1, float& t2) const;

private:
    Vector3 center;
    float radius;
};

#endif
