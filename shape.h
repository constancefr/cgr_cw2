#ifndef SHAPE_H
#define SHAPE_H

#include "ray.h"
#include <memory>

// Abstract Shape class
class Shape {
public:
    virtual bool intersects(const ray& r, double& t_hit) const = 0; // Returns true if the ray intersects
    virtual ~Shape() {}
};

#endif
