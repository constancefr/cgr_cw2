#ifndef RAY_H
#define RAY_H

#include "vector3.h"

class ray {
public:
    vector3 origin;
    vector3 direction;

    ray(const vector3& origin, const vector3& direction) : origin(origin), direction(direction) {}

    vector3 at(double t) const { return origin + direction * t; }

    vector3 get_direction() const { return direction; }

    vector3 get_origin() const { return origin; }
};

#endif
