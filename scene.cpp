#include "scene.h"
#include "sphere.h"
#include "triangle.h"
#include "cylinder.h"

bool Scene::intersects(const ray& r, double& t_hit, std::shared_ptr<Shape>& hit_shape) const {
    bool hit = false;
    double closest_t = std::numeric_limits<double>::max();
    for (const auto& shape : shapes) {
        double t = 0;
        if (shape->intersects(r, t) && t < closest_t) {
            closest_t = t;
            hit = true;
            hit_shape = shape;
        }
    }
    t_hit = closest_t;
    return hit;
}

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
