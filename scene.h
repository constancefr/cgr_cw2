#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <memory>
#include "vector3.h"
#include "ray.h"
#include "shape.h"
#include "triangle.h"

class Scene {
public:
    vector3 backgroundcolor;
    std::vector<std::shared_ptr<Shape>> shapes;

    Scene(const vector3& background_color) : backgroundcolor(background_color) {}

    void add_shape(const std::shared_ptr<Shape>& shape) {
        shapes.push_back(shape);
    }

    // Returns true if ray hits any object
    bool intersects(const ray& r, double& t_hit) const {
        bool hit = false;
        double closest_t = std::numeric_limits<double>::max();
        for (const auto& shape : shapes) {
            double t = 0;
            if (shape->intersects(r, t) && t < closest_t) {
                closest_t = t;
                hit = true;
            }
        }
        t_hit = closest_t;
        return hit;
    }
};

#endif
