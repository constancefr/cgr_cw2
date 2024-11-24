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

/* --------------- Utils --------------- */

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
/* ------------------------------------- */

void render(const Scene& scene, const Camera& camera, int image_width, int image_height, int max_depth, int samples_per_pixel, std::function<vector3(const vector3&)> tone_mapping, std::ofstream& outfile) {
    #pragma omp parallel for schedule(dynamic)  // Parallelize rendering if OpenMP is available
    for (int y = 0; y < image_height; ++y) {
        for (int x = 0; x < image_width; ++x) {
            vector3 pixel_color(0.0, 0.0, 0.0); // Final pixel color

            if (scene.enable_antialiasing) {
                // Antialiasing logic: Multi-sample and average
                for (int s = 0; s < samples_per_pixel; ++s) {
                    // Create jitter
                    double u_offset = random_double(-1.0, 1.0);
                    double v_offset = random_double(-1.0, 1.0);

                    auto [u, v] = normalize_pixel(x + u_offset, y+ v_offset, image_width, image_height);
                    ray r = camera.get_ray(u, v);

                    double t_hit;
                    std::shared_ptr<Shape> hit_shape;
                    vector3 sample_color = scene.backgroundcolor; // default

                    sample_color = scene.shade(r, 8);
                    pixel_color += sample_color; // Accumulate sample colors
                }

                // Average the accumulated color
                pixel_color = pixel_color / (samples_per_pixel);
                
            } else {
                // No antialiasing: Single ray per pixel
                auto [u, v] = normalize_pixel(x, y, image_width, image_height);
                ray r = camera.get_ray(u, v);

                double t_hit;
                std::shared_ptr<Shape> hit_shape;
                pixel_color = scene.backgroundcolor;

                pixel_color = scene.shade(r, 8);
            }

            // Apply tone mapping and gamma correction
            pixel_color = tone_mapping ? tone_mapping(pixel_color) : pixel_color;

            // Write the final color to the output
            write_colour(outfile, pixel_color);
        }
    }
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
    Scene scene(vector3(0, 0, 0));  // default colour
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
    int samples_per_pixel = 4; // default
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--bvh") {
            scene.use_bvh = true;
        } else if (arg == "--aa") {
            scene.enable_antialiasing = true;

            // Check if a subsequent argument specifies the number of samples
            if (i + 1 < argc) {
                std::string next_arg = argv[i + 1];
                try {
                    samples_per_pixel = std::stoi(next_arg);
                    ++i;
                } catch (std::invalid_argument&) {
                    std::cerr << "Invalid argument for antialiasing samples count: " << next_arg << "\n";
                }
            }
        }
    }

    // Build BVH by creating a tree from the list of shapes in the scene
    if (scene.use_bvh) {
        scene.build_bvh();
    }

    // Render image
    const int image_width = camera_json["width"];
    const int image_height = camera_json["height"];
    std::ofstream outfile("images/image.ppm");
    if (!outfile.is_open()) {
        std::cerr << "Error: Could not open output file.\n";
        return 1;
    }
    outfile << "P3\n" << image_width << " " << image_height << "\n255\n";

    auto start_time = std::chrono::high_resolution_clock::now();

    render(scene, camera, image_width, image_height, 8, samples_per_pixel, tone_mapping, outfile);

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_time = end_time - start_time;
    
    std::cout << "Render completed in: " << elapsed_time.count() << " seconds.\n";
    std::cout << "BVH enabled: " << (scene.use_bvh ? "Yes" : "No") << "\n";
    std::cout << "Antialiasing applied: " << (scene.enable_antialiasing ? "Yes" : "No") << "\n";

    outfile.close();
    
    return 0;
}
