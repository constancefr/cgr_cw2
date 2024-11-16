#include "scene.h"
#include "sphere.h"
#include "triangle.h"
#include "cylinder.h"

// Helper function to parse materials
Material parse_material(const nlohmann::json& material_json) {
    Material m;
    m.ks = material_json["ks"];
    m.kd = material_json["kd"];
    m.specularexponent = material_json["specularexponent"];
    m.diffusecolor = vector3(material_json["diffusecolor"][0], material_json["diffusecolor"][1], material_json["diffusecolor"][2]);
    m.specularcolor = vector3(material_json["specularcolor"][0], material_json["specularcolor"][1], material_json["specularcolor"][2]);
    m.isreflective = material_json["isreflective"];
    m.isrefractive = material_json["isrefractive"];
    m.reflectivity = material_json["reflectivity"];
    m.refractiveindex = material_json["refractiveindex"];
    return m;
}

void Scene::load_from_json(const nlohmann::json& scene_json) {
    // Set background color
    backgroundcolor = vector3(scene_json["backgroundcolor"][0], scene_json["backgroundcolor"][1], scene_json["backgroundcolor"][2]);

    // Add shapes
    for (const auto& shape_data : scene_json["shapes"]) {
        Material material = parse_material(shape_data["material"]);  // Parse material

        std::shared_ptr<Shape> shape = nullptr;
        if (shape_data["type"] == "sphere") {
            shape = std::make_shared<Sphere>(
                vector3(shape_data["center"][0], shape_data["center"][1], shape_data["center"][2]),
                shape_data["radius"],
                material
            );
        } else if (shape_data["type"] == "triangle") {
            shape = std::make_shared<Triangle>(
                vector3(shape_data["v0"][0], shape_data["v0"][1], shape_data["v0"][2]),
                vector3(shape_data["v1"][0], shape_data["v1"][1], shape_data["v1"][2]),
                vector3(shape_data["v2"][0], shape_data["v2"][1], shape_data["v2"][2]),
                material
            );
        } else if (shape_data["type"] == "cylinder") {
            shape = std::make_shared<Cylinder>(
                vector3(shape_data["center"][0], shape_data["center"][1], shape_data["center"][2]),
                vector3(shape_data["axis"][0], shape_data["axis"][1], shape_data["axis"][2]),
                shape_data["radius"],
                shape_data["height"],
                material
            );
        }

        if (shape) {
            add_shape(shape);
        }
    }
}

bool Scene::intersects(const ray& r, double& t_hit, std::shared_ptr<Shape>& hit_shape, double max_t) const {
    bool hit = false;
    double closest_t = max_t; // Only check up to max_t to avoid hitting objects beyond the light source
    for (const auto& shape : shapes) {
        double t = 0;
        if (shape->intersects(r, t) && t < closest_t && t > 1e-4) { // Avoid self-intersection with epsilon (1e-4)
            closest_t = t;
            hit = true;
            hit_shape = shape;
        }
    }
    t_hit = closest_t;
    return hit;
}

vector3 Scene::shade(
    const ray& r, 
    const vector3& hit_point, 
    const vector3& normal, 
    const Material& material
) const {
    vector3 color(0.0, 0.0, 0.0); // Initialize the color
    vector3 view_dir = -r.direction.unit(); // Direction to the viewer

    for (const auto& light : lights) {
        vector3 light_dir = (light.position - hit_point).unit(); // Direction to the light source

        // Shadow ray setup
        ray shadow_ray(hit_point, light_dir);
        double t_hit;
        std::shared_ptr<Shape> shadow_hit_shape;

        // Check for occlusion
        if (intersects(shadow_ray, t_hit, shadow_hit_shape, (light.position - hit_point).length())) {
            continue; // Skip this light if the shadow ray is blocked
        }

        // Diffuse component
        double diffuse_intensity = std::max(0.0, normal.dot(light_dir));
        vector3 diffuse = material.kd * diffuse_intensity * material.diffusecolor * light.intensity;

        // Specular component (Blinn-Phong)
        vector3 halfway_dir = (light_dir + view_dir).unit();
        double spec_intensity = std::pow(std::max(0.0, normal.dot(halfway_dir)), material.specularexponent);
        vector3 specular = material.ks * spec_intensity * material.specularcolor * light.intensity;

        // Accumulate contributions
        color += diffuse + specular;
    }

    return color;
}

