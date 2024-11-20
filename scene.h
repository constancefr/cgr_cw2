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

struct Light {
    vector3 position;
    vector3 intensity;
};

class Scene {

public:
    vector3 backgroundcolor;
    std::vector<std::shared_ptr<Shape>> shapes;
    std::vector<Light> lights;

    Scene(const vector3& background_color) : backgroundcolor(background_color) {}

    // Load scene from JSON configuration
    void load_from_json(const nlohmann::json& scene_json);

    void add_shape(const std::shared_ptr<Shape>& shape) {
        shapes.push_back(shape);
    }

    void add_light(const Light& light) {
        lights.push_back(light);
    }

    // Returns true if ray hits any object
    bool intersects(const ray& r, double& t_hit, std::shared_ptr<Shape>& hit_shape, double max_t) const;

    vector3 compute_blinn_phong(
        const vector3& point,
        const vector3& normal,
        const vector3& view_dir,
        const Material& material
    ) const;

    vector3 compute_reflection(
        const ray& r,
        const vector3& hit_point,
        const vector3& normal,
        const Material& material,
        int depth
    ) const;

    vector3 shade(const ray& r, const vector3& hit_point, const vector3& normal, const Material& material, int depth) const;

    vector3 compute_refracted_direction(
        const vector3& incident,
        const vector3& normal,
        double ior_in,
        double ior_out
    ) const;

};

#endif
