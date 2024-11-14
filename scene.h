#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <memory>
#include <string>
#include "vector3.h"
#include "ray.h"
#include "shape.h"
#include "triangle.h"
#include "libs/json.hpp"

class Scene {
public:
    vector3 backgroundcolor;
    std::vector<std::shared_ptr<Shape>> shapes;

    Scene(const vector3& background_color) : backgroundcolor(background_color) {}

    void add_shape(const std::shared_ptr<Shape>& shape) {
        shapes.push_back(shape);
    }

    // Returns true if ray hits any object
    bool intersects(const ray& r, double& t_hit, std::shared_ptr<Shape>& hit_shape) const;

    // Load scene from JSON configuration
    void load_from_json(const nlohmann::json& scene_json);
};

#endif
