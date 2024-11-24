#include <iostream>
#include <fstream>
#include <random>
#include "libs/json.hpp"
#include "vector3.h"
#include "colour.h"
#include "ray.h"
#include "camera.h"
#include "scene.h"
#include "sphere.h"
#include "triangle.h"
#include "cylinder.h"
#include "tone_mapping.h"

using json = nlohmann::json;

// double max_t = std::numeric_limits<double>::max(); // No upper bound for primary rays
double max_t = 1000.0;

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
// std::pair<double, double> normalize_pixel(int i, int j, int image_width, int image_height) {
//     double u = double(i) / (image_width - 1);  // Horizontal coordinate (0.0 to 1.0)
//     double v = double(j) / (image_height - 1); // Vertical coordinate (0.0 to 1.0)
//     return {u, v};
// }

// Center normalised coordinates inside pixel
std::pair<double, double> normalize_pixel(int i, int j, int width, int height) {
    return { (i + 0.5) / width, (j + 0.5) / height }; // Center pixel by default
}


double random_double(double min, double max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(min, max);
    return dis(gen);
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
    if (scene_json.contains("lightsources")) {
        for (const auto& light : scene_json["lightsources"]) {
            scene.add_light(Light{
                vector3(light["position"][0], light["position"][1], light["position"][2]),
                vector3(light["intensity"][0], light["intensity"][1], light["intensity"][2])
            });
        }
    }

    // Tone mapping
    std::function<vector3(const vector3&)> tone_mapping = nullptr;
    if (camera_json.contains("tone_mapping")) {
        std::string tone_mapping_str = camera_json["tone_mapping"];

        if (tone_mapping_str == "reinhard") {
            tone_mapping = reinhard_tone_mapping;
        } else if (tone_mapping_str == "aces") {
            tone_mapping = aces_tone_mapping;
        } else {
            throw std::invalid_argument("Unknown tone mapping method: " + tone_mapping_str);
        }
    } else { // Default to exposure tone mapping
        float exposure = camera_json["exposure"];
        tone_mapping = [exposure](const vector3& color) {
            return exposure_tone_mapping(color, exposure);
        };
    }

    // Parse command-line argument for flags
    int samples_per_pixel = 4;        // Default samples per pixel
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--bvh") {
            scene.use_bvh = true; // Enable BVH
        } else if (arg == "--aa") {
            scene.enable_antialiasing = true; // Enable antialiasing

            // Check if a subsequent argument specifies the number of samples
            if (i + 1 < argc) {
                std::string next_arg = argv[i + 1];
                try {
                    samples_per_pixel = std::stoi(next_arg); // Parse samples count
                    ++i; // Skip the parsed argument
                } catch (std::invalid_argument&) {
                    // Not a valid integer, keep default samples_per_pixel
                }
            }
        }
    }

    // Build BVH by creating a tree from the list of shapes in the scene
    if (scene.use_bvh) {
        scene.build_bvh();
    }
    
    auto start_time = std::chrono::high_resolution_clock::now();

    // Render image
    const int image_width = camera_json["width"];
    const int image_height = camera_json["height"];
    std::ofstream outfile("image.ppm");
    if (!outfile.is_open()) {
        std::cerr << "Error: Could not open output file.\n";
        return 1;
    }
    outfile << "P3\n" << image_width << " " << image_height << "\n255\n";

    // Parse through each pixel
    for (int j = 0; j < image_height; ++j) {
        for (int i = 0; i < image_width; ++i) {
            vector3 pixel_color(0.0, 0.0, 0.0); // Final pixel color

            if (scene.enable_antialiasing) {
                // Antialiasing logic: Multi-sample and average
                for (int s = 0; s < samples_per_pixel; ++s) {
                    // Create jitter
                    double u_offset = random_double(-1.0, 1.0);
                    double v_offset = random_double(-1.0, 1.0);

                    auto [u, v] = normalize_pixel(i + u_offset, j + v_offset, image_width, image_height);
                    ray r = camera.get_ray(u, v);

                    double t_hit;
                    std::shared_ptr<Shape> hit_shape;
                    vector3 sample_color = scene.backgroundcolor; // default

                    if (scene.intersects(r, t_hit, hit_shape, max_t)) {
                        vector3 hit_point = r.origin + t_hit * r.direction;
                        vector3 normal = hit_shape->get_normal(hit_point);
                        sample_color = scene.shade(r, hit_point, normal, *hit_shape, 8);
                    }

                    pixel_color += sample_color; // Accumulate sample colors
                }

                // Average the accumulated color
                pixel_color = pixel_color / (samples_per_pixel);
                
            } else {
                // No antialiasing: Single ray per pixel
                auto [u, v] = normalize_pixel(i, j, image_width, image_height);
                ray r = camera.get_ray(u, v);

                double t_hit;
                std::shared_ptr<Shape> hit_shape;
                pixel_color = scene.backgroundcolor;

                if (scene.intersects(r, t_hit, hit_shape, max_t)) {
                    vector3 hit_point = r.origin + t_hit * r.direction;
                    vector3 normal = hit_shape->get_normal(hit_point);
                    pixel_color = scene.shade(r, hit_point, normal, *hit_shape, 8);
                }
            }

            // Apply tone mapping and gamma correction
            pixel_color = tone_mapping ? tone_mapping(pixel_color) : pixel_color;
            // pixel_color = gamma_correction(pixel_color, 1.0f / 2.2f);

            // Write the final color to the output
            write_colour(outfile, pixel_color);
        }
    }



    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_time = end_time - start_time;

    std::cout << "Render completed in: " << elapsed_time.count() << " seconds.\n";
    std::cout << "BVH enabled: " << (scene.use_bvh ? "Yes" : "No") << "\n";
    std::cout << "Antialiasing applied: " << (scene.enable_antialiasing ? "Yes" : "No") << "\n";

    outfile.close();
    
    return 0;
}
