#include "scene.h"
#include "utils.h"

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
    // Set render mode
    if (scene_json.contains("rendermode")) {
        render_mode = parse_render_mode(scene_json["rendermode"]);
    } else {
        render_mode = RenderMode::Binary;
    }
    set_render_mode(render_mode);

    // Set background color
    backgroundcolor = vector3(scene_json["backgroundcolor"][0], scene_json["backgroundcolor"][1], scene_json["backgroundcolor"][2]);

    // Add lights
    if (scene_json.contains("lightsources")) {
        for (const auto& light_data : scene_json["lightsources"]) {
            Light light;
            light.position = vector3(light_data["position"][0], light_data["position"][1], light_data["position"][2]);
            light.intensity = vector3(light_data["intensity"][0], light_data["intensity"][1], light_data["intensity"][2]);

            if (light_data["type"] == "pointlight") {
                light.type = LightType::Point;
            } else if (light_data["type"] == "arealight") {
                light.type = LightType::Area;
                light.u = vector3(light_data["u"][0], light_data["u"][1], light_data["u"][2]).unit();
                light.v = vector3(light_data["v"][0], light_data["v"][1], light_data["v"][2]).unit();
                light.width = light_data["width"];
                light.height = light_data["height"];
            }

            add_light(light);
        }
    }

    // Add shapes
    for (const auto& shape_data : scene_json["shapes"]) {
        Material material;
        if (shape_data.contains("material")) {
            material = parse_material(shape_data["material"]);
        } else {
            material = Material();  // default material
        }

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
            // Load texture to the shape's material if available
            if (shape_data.contains("material") && shape_data["material"].contains("texture_file")) {
                std::string texture_file = shape_data["material"]["texture_file"];
                std::shared_ptr<Image> texture = std::make_shared<Image>(texture_file);

                shape->material.texture = texture;
                shape->material.texture_file = texture_file;
            }

            add_shape(shape);
        }
    }
}

// Iterates over all shapes in the scene and checks for intersections with the given ray.
bool Scene::brute_force_intersects(const ray& r, double& t_hit, std::shared_ptr<Shape>& hit_shape, double max_t) const {
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


/* --------------- Shading / reflection / refraction functions --------------- */

vector3 Scene::shade(
    const ray& r, 
    int nbounces
) const {
    switch (render_mode) {
        // TODO!!
        case RenderMode::Binary:
            return shade_binary(r);
        case RenderMode::BlinnPhong:
            return shade_blinn_phong(r, nbounces);
        default:
            throw std::runtime_error("Unsupported render mode.");
    }
}

// shade_binary: simple binary shading
vector3 Scene::shade_binary(const ray& r) const {
    double t_hit;
    std::shared_ptr<Shape> hit_shape;
    if (!intersects(r, t_hit, hit_shape, std::numeric_limits<double>::max())) {
        return vector3(0.0, 0.0, 0.0); // black
    }
    return vector3(1.0, 0.0, 0.0); // red
}

vector3 Scene::shade_blinn_phong(const ray& r, int nbounces) const {
    double t_hit;
    std::shared_ptr<Shape> hit_shape;
    if (!intersects(r, t_hit, hit_shape, std::numeric_limits<double>::max())) {
        return backgroundcolor; // No intersection, return background color
    }

    vector3 hit_point = r.origin + t_hit * r.direction;
    vector3 normal = hit_shape->get_normal(hit_point);
    return shade_surface(r, hit_point, normal, hit_shape->material, *hit_shape, nbounces);
}

vector3 Scene::shade_surface(
    const ray& r,
    const vector3& hit_point,
    const vector3& normal,
    const Material& material,
    const Shape& shape,
    int nbounces
) const {
    vector3 view_dir = -r.direction.unit();

    // Local shading
    vector3 local_color = compute_blinn_phong(hit_point, normal, view_dir, material, shape);

    // Reflection
    // vector3 reflection_color = compute_reflection(r, hit_point, normal, material, nbounces);
    vector3 reflection_color = compute_reflection(r, hit_point, normal, material, nbounces - 1);

    // Refraction (if implemented)
    vector3 refraction_color = material.isrefractive
        // ? compute_refraction(r, hit_point, normal, material, nbounces)
        ? compute_refraction(r, hit_point, normal, material, nbounces - 1)
        : vector3(0.0, 0.0, 0.0);

    // Combine components
    return local_color + reflection_color + refraction_color;
}

vector3 Scene::compute_blinn_phong(
    const vector3& point,
    const vector3& normal,
    const vector3& view_dir,
    const Material& material,
    const Shape& shape
) const {
    vector3 color(0.0, 0.0, 0.0);

    auto uv = shape.get_uv(point);
    vector3 texture_color = material.texture
        ? material.texture->get_color_at_uv(uv.first, uv.second)
        : material.diffusecolor;

    for (const auto& light : lights) {
        if (light.type == LightType::Point) {
            vector3 light_dir = (light.position - point).unit();
            vector3 half_vector = (view_dir + light_dir).unit();

            double diff = std::max(0.0, normal.dot(light_dir));
            vector3 diffuse = material.kd * diff * texture_color * light.intensity;

            double spec = std::pow(std::max(0.0, normal.dot(half_vector)), material.specularexponent);
            vector3 specular = material.ks * spec * material.specularcolor * light.intensity;

            double shadow_factor = compute_shadow_factor(point, light.position);

            color += shadow_factor * (diffuse + specular);

        } else if (light.type == LightType::Area) {
            int num_samples = 16; // Adjust for quality vs. performance
            vector3 area_color(0.0, 0.0, 0.0);

            for (int i = 0; i < num_samples; ++i) {
                vector3 sample_point = light.sample_point_on_surface(); // random sample
                vector3 light_dir = (sample_point - point).unit();

                // Shadow factor
                double shadow_factor = compute_shadow_factor(point, sample_point);

                // Diffuse component
                double diff = std::max(0.0, normal.dot(light_dir));
                vector3 diffuse = material.kd * diff * texture_color * light.intensity / num_samples;

                // Specular component
                vector3 half_vector = (view_dir + light_dir).unit();
                double spec = std::pow(std::max(0.0, normal.dot(half_vector)), material.specularexponent);
                vector3 specular = material.ks * spec * material.specularcolor * light.intensity / num_samples;

                area_color += shadow_factor * (diffuse + specular);
            }
            color += area_color;
        }
        
    }

    return color;
}

double Scene::compute_shadow_factor(const vector3& point, const vector3& light_position) const {
    vector3 light_dir = (light_position - point).unit();
    ray shadow_ray(point + light_dir * 0.001, light_dir); // Offset to avoid self-intersection

    double t_shadow;
    std::shared_ptr<Shape> shadow_hit_shape;
    if (intersects(shadow_ray, t_shadow, shadow_hit_shape, (light_position - point).length())) {
        return 0.1; // In shadow
    }
    return 1.0; // Fully lit
}

vector3 Scene::compute_reflection(
    const ray& r,
    const vector3& hit_point,
    const vector3& normal,
    const Material& material,
    int nbounces
) const {
    if (nbounces <= 0 || !material.isreflective) return vector3(0.0, 0.0, 0.0);

    vector3 reflect_dir = r.direction - 2 * (normal.dot(r.direction)) * normal;
    ray reflect_ray(hit_point + reflect_dir * 0.001, reflect_dir);

    return shade_blinn_phong(reflect_ray, nbounces - 1) * material.reflectivity;
}

vector3 Scene::compute_refraction(
    const ray& r,
    const vector3& hit_point,
    const vector3& normal,
    const Material& material,
    int nbounces
) const {
    // Refraction indices
    double eta = material.refractiveindex; // Refractive index of the material
    double eta_inv = 1.0 / eta;

    // Compute the unit direction of the incident ray
    vector3 incident_dir = r.direction.unit();

    // Cosine of the angle between the incident ray and the surface normal
    double cos_theta_i = -normal.dot(incident_dir);

    // Determine if the ray is entering or exiting the medium
    vector3 refract_normal = normal;
    if (cos_theta_i < 0.0) {
        // Exiting the medium
        cos_theta_i = -cos_theta_i;
        refract_normal = -normal;
        eta = eta_inv;
    }

    // Calculate the sine squared of the refracted angle using Snell's Law
    double sin2_theta_t = eta * eta * (1.0 - cos_theta_i * cos_theta_i);

    // Check for total internal reflection (TIR)
    if (sin2_theta_t > 1.0) {
        // TIR occurs; no refraction, return black color
        return vector3(0.0, 0.0, 0.0);
    }

    // Calculate the cosine of the refracted angle
    double cos_theta_t = sqrt(1.0 - sin2_theta_t);

    // Compute the refracted direction
    vector3 refract_dir = eta * incident_dir + (eta * cos_theta_i - cos_theta_t) * refract_normal;
    refract_dir = refract_dir.unit();

    // Generate the refracted ray
    ray refracted_ray(hit_point + refract_dir * 1e-4, refract_dir); // Offset to avoid self-intersection

    // Calculate the refraction color by recursively shading the refracted ray
    vector3 refraction_color = shade_blinn_phong(refracted_ray, nbounces + 1); // + 1 because we are entering a new medium??

    // Attenuate the refraction color by the material's transparency
    return refraction_color * material.transparency;
}

// Randomly sample a point on the surface of the area light
vector3 Light::sample_point_on_surface() const {
    if (type != LightType::Area) {
        throw std::runtime_error("Sampling only supported for area lights");
    }
    double rand_u = random_double(0.0, 1.0);
    double rand_v = random_double(0.0, 1.0);
    return position + (rand_u * width * u) + (rand_v * height * v);
}
