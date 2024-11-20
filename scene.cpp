#include "scene.h"
#include "sphere.h"
#include "triangle.h"
#include "cylinder.h"

// RenderMode parse_render_mode(const std::string& mode_str) {
//     if (mode_str == "binary") return RenderMode::Binary;
//     return RenderMode::BlinnPhong;
//     // throw std::invalid_argument("Unknown render mode: " + mode_str);
// }

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
                // multiply height by 2 to get the full height of the cylinder
                // shape_data["height"].get<double>() * 2,
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

vector3 Scene::compute_blinn_phong(
    const vector3& point,           // Point on the surface
    const vector3& normal,          // Surface normal at the point
    const vector3& view_dir,        // Direction from point to camera
    const Material& material        // Material of the surface
    // const std::vector<Light>& lights // Lights in the scene
) const {
    vector3 color(0.0, 0.0, 0.0);  // Accumulated color

    for (const auto& light : lights) {
        // Calculate vectors
        vector3 light_dir = (light.position - point).unit(); // Direction to the light
        vector3 half_vector = (view_dir + light_dir).unit(); // Halfway vector

        // Diffuse contribution
        double diff = std::max(0.0, normal.dot(light_dir));
        vector3 diffuse = material.kd * diff * material.diffusecolor * light.intensity;

        // Specular contribution
        double spec = std::pow(std::max(0.0, normal.dot(half_vector)), material.specularexponent);
        vector3 specular = material.ks * spec * material.specularcolor * light.intensity;

        // Accumulate contributions
        color += diffuse + specular;
    }

    return color;
}

vector3 Scene::compute_reflection(
    const ray& r,
    const vector3& hit_point,
    const vector3& normal,
    const Material& material,
    int depth
) const {
    if (depth <= 0 || !material.isreflective) {
        return vector3(0.0, 0.0, 0.0);
    }

    // Calculate reflection direction
    vector3 reflect_dir = r.direction - 2 * (normal.dot(r.direction)) * normal;

    // Create a reflection ray
    ray reflection_ray(hit_point + reflect_dir * 0.001, reflect_dir); // Slight offset to avoid self-intersection

    // Check for intersection of the reflection ray
    double t_hit_reflection;
    std::shared_ptr<Shape> hit_shape_reflection;
    
    if (intersects(reflection_ray, t_hit_reflection, hit_shape_reflection, std::numeric_limits<double>::max())) {
        // Get the color of the reflection
        vector3 hit_point_reflection = reflection_ray.origin + t_hit_reflection * reflection_ray.direction;
        vector3 normal_reflection = hit_shape_reflection->get_normal(hit_point_reflection);
        vector3 view_dir_reflection = -reflection_ray.direction.unit();

        // Compute the color for this reflection ray recursively
        vector3 reflection_color = compute_blinn_phong(hit_point_reflection, normal_reflection, view_dir_reflection, hit_shape_reflection->material);

        // Combine the reflection color with the original color using reflectivity
        return reflection_color * material.reflectivity;
    }

    // If no reflection is found, return black
    return vector3(0.0, 0.0, 0.0);
}

vector3 Scene::compute_refracted_direction(
    const vector3& incident,  // Incident ray direction (normalized)
    const vector3& normal,    // Surface normal at the point of intersection
    double ior_in,            // Refractive index of the medium the ray is coming from
    double ior_out            // Refractive index of the medium the ray is entering
) const {
    double cos_theta_i = -incident.dot(normal); // Angle of incidence
    double eta = ior_in / ior_out;              // Ratio of refractive indices
    double sin2_theta_t = eta * eta * (1.0 - cos_theta_i * cos_theta_i);

    // Check for total internal reflection
    if (sin2_theta_t > 1.0) {
        return vector3(0.0, 0.0, 0.0); // No refraction, returning a zero vector
    }

    double cos_theta_t = std::sqrt(1.0 - sin2_theta_t); // Angle of refraction
    return eta * incident + (eta * cos_theta_i - cos_theta_t) * normal;
}

vector3 Scene::shade(
    const ray& r, 
    const vector3& hit_point, 
    const vector3& normal, 
    const Material& material, 
    int depth
) const {
    switch (render_mode) {
        // case RenderMode::Binary:
            // return shade_binary(r, hit_point, normal, material);
        case RenderMode::BlinnPhong:
            return shade_blinn_phong(r, hit_point, normal, material, depth);
        default:
            throw std::runtime_error("Unsupported render mode.");
    }
}

vector3 Scene::shade_blinn_phong(
    const ray& r, 
    const vector3& hit_point, 
    const vector3& normal, 
    const Material& material,
    int depth
) const {
    vector3 color(0.0, 0.0, 0.0); // Initialize the color
    vector3 view_dir = -r.direction.unit(); // Direction to the viewer

    for (const auto& light : lights) {
        vector3 light_dir = (light.position - hit_point).unit(); // Direction to the light source
        ray shadow_ray(hit_point + light_dir * 0.001, light_dir);

        double t_hit;
        std::shared_ptr<Shape> shadow_hit_shape;

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

    // Reflection component
    if (material.isreflective) {
        vector3 reflection = compute_reflection(r, hit_point, normal, material, depth - 1);
        color += reflection;
    }

    // Refraction component
    if (material.isrefractive && depth > 0) {
        double ior_in = 1.0; // Air's refractive index
        double ior_out = material.refractiveindex;

        // Determine the direction of the refracted ray
        vector3 refracted_dir = compute_refracted_direction(view_dir, normal, ior_in, ior_out);

        if (refracted_dir.length() > 0.0) { // No total internal reflection
            ray refracted_ray(hit_point + refracted_dir * 0.001, refracted_dir);

            double t_hit_refracted;
            std::shared_ptr<Shape> hit_shape_refracted;
            if (intersects(refracted_ray, t_hit_refracted, hit_shape_refracted, std::numeric_limits<double>::infinity())) {
                vector3 hit_point_refracted = refracted_ray.origin + t_hit_refracted * refracted_ray.direction;
                vector3 normal_refracted = hit_shape_refracted->get_normal(hit_point_refracted);
                vector3 view_dir_refracted = -refracted_ray.direction.unit();

                // Compute the refracted color
                vector3 refracted_color = shade(
                    refracted_ray, hit_point_refracted, normal_refracted, hit_shape_refracted->material, depth - 1
                );

                // Combine with existing color using transparency
                color += material.transparency * refracted_color;
            }
        }
    }


    return color;
}

