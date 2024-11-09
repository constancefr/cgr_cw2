#include <iostream>
#include "vector3.h"
#include "colour.h"
#include "ray.h"
#include "camera.h"

// Normalize pixel coordinates
std::pair<double, double> normalize_pixel(int i, int j, int image_width, int image_height) {
    double u = double(i) / (image_width - 1);  // Horizontal coordinate (0.0 to 1.0)
    double v = double(j) / (image_height - 1); // Vertical coordinate (0.0 to 1.0)
    return {u, v};
}

vector3 ray_colour(const ray& r) {
    vector3 unit_dir = r.direction.unit();
    double t = 0.5 * (unit_dir.y + 1.0); // Map y value to [0,1]
    return vector3(1.0, 1.0, 1.0) * (1.0 - t) + vector3(0.5, 0.7, 1.0) * t; // Blend white to blue
}

int main() {
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

    return 0;
}
