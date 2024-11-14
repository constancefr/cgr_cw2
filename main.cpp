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

    // Render binary image
    const int image_width = camera_json["width"];
    const int image_height = camera_json["height"];
    std::ofstream outfile("image.ppm");
    if (!outfile.is_open()) {
        std::cerr << "Error: Could not open output file.\n";
        return 1;
    }
    outfile << "P3\n" << image_width << " " << image_height << "\n255\n";
    std::cout << "PPM file header: \n" << "P3\n" << image_width << " " << image_height << "\n255\n";
    
    for (int j = 0; j < image_height; ++j) {
    // for (int j = image_height - 1; j >= 0; --j) { // from top to bottom?
        for (int i = 0; i < image_width; ++i) {// from left to right
            auto [u, v] = normalize_pixel(i, j, image_width, image_height);
            ray r = camera.get_ray(u, v);

            double t_hit;
            std::shared_ptr<Shape> hit_shape;

            if (scene.intersects(r, t_hit, hit_shape)) {
                // Use the material's diffuse color for now
                write_colour(outfile, hit_shape->material.diffusecolor);
            } else {
                // Use the scene's background color
                write_colour(outfile, scene.backgroundcolor);
            }
        }
    }

    outfile.close();
    
    return 0;
}
