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

double max_t = std::numeric_limits<double>::max(); // No upper bound for primary rays

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


int main(int argc, char* argv[]) {

    // Load JSON
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <jsons/binary_primitves.json>\n";
        return 1;
    }

    json config = load_json(argv[1]);
    // int nbounces = config["nbounces"];
    auto camera_json = config["camera"];
    auto scene_json = config["scene"];
    
    // Parse scene
    Scene scene(vector3(0, 0, 0));  // Initialize with a default color
    scene.load_from_json(scene_json);
    RenderMode rendermode = scene.parse_render_mode(config["rendermode"]);
    scene.set_render_mode(rendermode);

    // Parse camera
    Camera camera(
        camera_json["width"],
        camera_json["height"],
        camera_json["fov"],
        vector3(camera_json["position"][0], camera_json["position"][1], camera_json["position"][2]),
        vector3(camera_json["lookAt"][0], camera_json["lookAt"][1], camera_json["lookAt"][2]),
        vector3(camera_json["upVector"][0], camera_json["upVector"][1], camera_json["upVector"][2])
    );

    

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
        for (int i = 0; i < image_width; ++i) { // From left to right
            auto [u, v] = normalize_pixel(i, j, image_width, image_height);
            ray r = camera.get_ray(u, v);

            double t_hit;
            std::shared_ptr<Shape> hit_shape;

            if (scene.intersects(r, t_hit, hit_shape, max_t)) {
                vector3 hit_point = r.origin + t_hit * r.direction;
                vector3 normal = hit_shape->get_normal(hit_point);

                // Calculate shading
                vector3 shaded_color = scene.shade(r, hit_point, normal, hit_shape->material, 3);
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
