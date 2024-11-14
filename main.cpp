#include <iostream>
#include <fstream>
#include "libs/json.hpp"
#include "vector3.h"
#include "colour.h"
#include "ray.h"
#include "camera.h"
#include "scene.h"
#include "sphere.h"
#include "triangle.h"
#include "cylinder.h"

using json = nlohmann::json;

json load_json(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open JSON file: " + filename);
    }
    json data;
    file >> data;
    
    return data;
}

// Normalize pixel coordinates
std::pair<double, double> normalize_pixel(int i, int j, int image_width, int image_height) {
    double u = double(i) / (image_width - 1);  // Horizontal coordinate (0.0 to 1.0)
    double v = double(j) / (image_height - 1); // Vertical coordinate (0.0 to 1.0)
    return {u, v};
}

vector3 compute_blinn_phong(
    const vector3& point,           // Point on the surface
    const vector3& normal,          // Surface normal at the point
    const vector3& view_dir,        // Direction from point to camera
    const Material& material,       // Material of the surface
    const std::vector<Light>& lights // Lights in the scene
) {
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


int main(int argc, char* argv[]) {

    // Load JSON
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <jsons/binary_primitves.json>\n";
        return 1;
    }

    json config = load_json(argv[1]);
    auto camera_json = config["camera"];
    auto scene_json = config["scene"];

    std::cerr << "Debug test\n";

    // Parse camera
    Camera camera(
        camera_json["width"],
        camera_json["height"],
        camera_json["fov"],
        vector3(camera_json["position"][0], camera_json["position"][1], camera_json["position"][2]),
        vector3(camera_json["lookAt"][0], camera_json["lookAt"][1], camera_json["lookAt"][2]),
        vector3(camera_json["upVector"][0], camera_json["upVector"][1], camera_json["upVector"][2])
    );

    // Parse scene
    Scene scene(vector3(0, 0, 0));  // Initialize with a default color
    scene.load_from_json(scene_json);

    // Parse lights
    for (const auto& light : scene_json["lightsources"]) {
        scene.add_light(Light{
            vector3(light["position"][0], light["position"][1], light["position"][2]),
            vector3(light["intensity"][0], light["intensity"][1], light["intensity"][2])
        });
    }

    // Render image
    const int image_width = camera_json["width"];
    const int image_height = camera_json["height"];
    std::ofstream outfile("image.ppm");
    if (!outfile.is_open()) {
        std::cerr << "Error: Could not open output file.\n";
        return 1;
    }
    outfile << "P3\n" << image_width << " " << image_height << "\n255\n";
    std::cout << "PPM file header: \n" << "P3\n" << image_width << " " << image_height << "\n255\n";
    
    // for (int j = image_height - 1; j >= 0; --j) { // from top to bottom?
    for (int j = 0; j < image_height; ++j) {
        for (int i = 0; i < image_width; ++i) { // from left to right
            auto [u, v] = normalize_pixel(i, j, image_width, image_height);
            ray r = camera.get_ray(u, v);

            double t_hit;
            std::shared_ptr<Shape> hit_shape;

            if (scene.intersects(r, t_hit, hit_shape)) {
                // Calculate intersection point and normal
                vector3 hit_point = r.origin + t_hit * r.direction;
                vector3 normal = hit_shape->get_normal(hit_point); // Implement `get_normal` for each shape

                // View direction (from hit point to camera)
                vector3 view_dir = -r.direction.unit();

                // Compute Blinn-Phong shading
                vector3 shaded_color = compute_blinn_phong(
                    hit_point, normal, view_dir, hit_shape->material, scene.lights
                );

                write_colour(outfile, shaded_color);
            } else {
                // Use the scene's background color
                write_colour(outfile, scene.backgroundcolor);
            }
        }
    }

    outfile.close();
    
    return 0;
}
