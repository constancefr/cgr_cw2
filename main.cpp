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
    double v = 1.0 - double(j) / (image_height - 1); // Vertical coordinate (0.0 to 1.0)
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
    Scene scene(vector3(scene_json["backgroundcolor"][0], scene_json["backgroundcolor"][1], scene_json["backgroundcolor"][2]));

    // Add shapes to scene
    for (const auto& shape : scene_json["shapes"]) {
        if (shape["type"] == "sphere") {
            auto sphere = std::make_shared<Sphere>(
                vector3(shape["center"][0], shape["center"][1], shape["center"][2]),
                shape["radius"]
            );
            scene.add_shape(sphere);
        } else if (shape["type"] == "triangle") {
            auto triangle = std::make_shared<Triangle>(
                vector3(shape["v0"][0], shape["v0"][1], shape["v0"][2]),
                vector3(shape["v1"][0], shape["v1"][1], shape["v1"][2]),
                vector3(shape["v2"][0], shape["v2"][1], shape["v2"][2])
            );
            scene.add_shape(triangle);
        } else if (shape["type"] == "cylinder") {  // Add support for cylinders
            auto cylinder = std::make_shared<Cylinder>(
                vector3(shape["center"][0], shape["center"][1], shape["center"][2]),
                vector3(shape["axis"][0], shape["axis"][1], shape["axis"][2]),
                shape["radius"],
                shape["height"]
            );
            scene.add_shape(cylinder);
        }
    }

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
        for (int i = 0; i < image_width; ++i) {
            auto [u, v] = normalize_pixel(i, j, image_width, image_height);
            ray r = camera.get_ray(u, v);

            double t_hit;
            if (scene.intersects(r, t_hit)) {
                // Hit: Red pixel
                write_colour(outfile, vector3(1.0, 0.0, 0.0));
            } else {
                // Miss: Blue pixel
                write_colour(outfile, vector3(0.0, 0.0, 1.0));
            }
        }
    }

    outfile.close();
    
    return 0;
}
