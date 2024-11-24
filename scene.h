#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <memory>
#include <string>
#include "vector3.h"
#include "ray.h"
#include "shape.h"
#include "sphere.h"
#include "triangle.h"
#include "cylinder.h"
#include "json.hpp"

enum class LightType {
    Point,
    Area
};

struct Light {
    LightType type; // point or area
    vector3 position; // centre of light source
    vector3 intensity;
    vector3 u; // u-axis for area light
    vector3 v; // v-axis for area light
    double width;
    double height;
    
    vector3 sample_point_on_surface() const;
};


enum class RenderMode {
        Binary,
        BlinnPhong
};

class Scene {
public:
    RenderMode render_mode;
    vector3 backgroundcolor;
    std::vector<std::shared_ptr<Shape>> shapes;
    std::vector<Light> lights;
    std::shared_ptr<BVH> bvh;
    bool use_bvh = false;
    bool enable_antialiasing = false;

    /* --------------- Scene parsing --------------- */

    RenderMode parse_render_mode(const std::string& mode_str) {
        if (mode_str == "binary") return RenderMode::Binary;
        return RenderMode::BlinnPhong;
    }

    void set_render_mode(RenderMode mode) {
        render_mode = mode;
    }

    Scene(const vector3& background_color) : backgroundcolor(background_color) {}

    void load_from_json(const nlohmann::json& scene_json);

    void add_shape(const std::shared_ptr<Shape>& shape) {
        shapes.push_back(shape);
    }

    void add_light(const Light& light) {
        lights.push_back(light);
    }

    /* --------------- BVH & intersection --------------- */
    void build_bvh() {
        if (use_bvh) {
            bvh = std::make_shared<BVH>(shapes);
        }
    }

    bool intersects(const ray& r, double& t_hit, std::shared_ptr<Shape>& hit_shape, double max_t) const {
        if (use_bvh) {
            return bvh->intersects(r, t_hit, hit_shape, max_t);
        }
        return brute_force_intersects(r, t_hit, hit_shape, max_t);
    }

    bool brute_force_intersects(const ray& r, double& t_hit, std::shared_ptr<Shape>& hit_shape, double max_t) const;


    /* --------------- Shading / reflection / refraction --------------- */

    vector3 shade(
        const ray& r, 
        int nbounces
    ) const;

    vector3 compute_blinn_phong(
        const vector3& point,
        const vector3& normal,
        const vector3& view_dir,
        const Material& material,
        const Shape& shape
    ) const;

    double compute_shadow_factor(const vector3& point, const vector3& light_position) const;
    // double compute_shadow_factor(const vector3& point, const Light& light) const;

    vector3 compute_reflection(
        const ray& r,
        const vector3& hit_point,
        const vector3& normal,
        const Material& material,
        int nbounces
    ) const;

    vector3 shade_surface(
        const ray& r,
        const vector3& hit_point,
        const vector3& normal,
        const Material& material,
        const Shape& shape,
        int nbounces
    ) const;

    vector3 shade_binary(const ray& r) const;

    vector3 shade_blinn_phong(const ray& r, int nbounces) const;

    vector3 compute_refraction(
        const ray& r_in,              // Incoming ray
        const vector3& hit_point,     // Point of intersection
        const vector3& normal,        // Surface normal
        const Material& material,     // Material of the hit object
        int nbounces                     // Recursion depth
    ) const;

    // KEPT IN NEW CPP
    vector3 compute_refracted_direction(
        const vector3& incident,
        const vector3& normal,
        double ior_in,
        double ior_out
    ) const;

};

#endif
