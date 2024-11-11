#include <iostream>
#include <fstream>
#include "libs/json.hpp"
#include "vector3.h"
#include "colour.h"
#include "ray.h"
#include "camera.h"
#include "scene.h"

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

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <jsons/binary_primitves.json>\n";
        return 1;
    }

    // Load JSON
    json config = load_json(argv[1]);
    // std::cout << "Loaded json file:\n" << config << "\n";

    // Parse camera
    auto camera_json = config["camera"];
    Camera camera(
        camera_json["width"],
        camera_json["height"],
        camera_json["fov"],
        vector3(camera_json["position"][0], camera_json["position"][1], camera_json["position"][2]),
        vector3(camera_json["lookAt"][0], camera_json["lookAt"][1], camera_json["lookAt"][2]),
        vector3(camera_json["upVector"][0], camera_json["upVector"][1], camera_json["upVector"][2])
    );
    // std::cout << "Parsed camera - camera origin: \n" << camera.origin << "\n";

    // Parse scene
    auto scene_json = config["scene"];
    Scene scene(vector3(scene_json["backgroundcolor"][0], scene_json["backgroundcolor"][1], scene_json["backgroundcolor"][2]));
    // std::cout << "Parsed scene - background colour: " << scene.backgroundcolor;

    // Image settings
    const int image_width = camera_json["width"];
    const int image_height = camera_json["height"];
    // std::cout << "image width: " << image_width << "\n255\n";

    // Open output file
    std::ofstream outfile("image.ppm");
    if (!outfile.is_open()) {
        std::cerr << "Error: Could not open output file.\n";
        return 1;
    }

    // Render
    outfile << "P3\n" << image_width << " " << image_height << "\n255\n";
    std::cout << "PPM file header: \n" << "P3\n" << image_width << " " << image_height << "\n255\n";
    
    for (int j = 0; j < image_height; ++j) {
        for (int i = 0; i < image_width; ++i) {
            auto [u, v] = normalize_pixel(i, j, image_width, image_height);
            ray r = camera.get_ray(u, v);

            vector3 colour = scene.backgroundcolor; // Default background color

            write_colour(outfile, colour);
        }
    }

    outfile.close();
    
    return 0;
}


/*
    // Image
    const double aspect_ratio = 16.0 / 9.0;
    const int image_width = 400;
    const int image_height = static_cast<int>(image_width / aspect_ratio);

    // Camera
    Camera camera(aspect_ratio, 2.0, 1.0);

    // Render
    std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";
    for (int j = 0; j < image_height; ++j) {
        for (int i = 0; i < image_width; ++i) {
            auto [u, v] = normalize_pixel(i, j, image_width, image_height);
            ray r = camera.get_ray(u, v);
            vector3 colour = ray_colour(r);
            write_colour(std::cout, colour);
        }
    }
*/

/*
vector3 ray_colour(const ray& r) {
    vector3 unit_dir = r.direction.unit();
    double t = 0.5 * (unit_dir.y + 1.0); // Map y value to [0,1]
    return vector3(1.0, 1.0, 1.0) * (1.0 - t) + vector3(0.5, 0.7, 1.0) * t; // Blend white to blue
}
*/
