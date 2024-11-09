#include <iostream>
#include "vector3.h"
#include "colour.h"
#include "ray.h"

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
    double viewport_height = 2.0;
    double viewport_width = aspect_ratio * (image_width / image_height);
    double focal_length = 1.0;

    vector3 origin(0, 0, 0);
    vector3 horizontal(viewport_width, 0, 0);
    vector3 vertical(0, -viewport_height, 0);
    vector3 upper_left_corner = origin - horizontal / 2 - vertical / 2 - vector3(0, 0, focal_length);

    // Render
    std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";

    for (int j = image_height - 1; j >= 0; --j) {
        for (int i = 0; i < image_width; ++i) {
            double u = double(i) / (image_width - 1);
            double v = double(j) / (image_height - 1);
            ray r(origin, lower_left_corner + horizontal * u + vertical * v - origin);
            vector3 colour = ray_colour(r);
            write_colour(std::cout, colour);
        
            // Track progress
            int currentPixel = j * width + i + 1;  // Current pixel number
            float progress = static_cast<float>(currentPixel) / totalPixels * 100;
            // Print progress every 5% or on the last pixel
            if (currentPixel % (totalPixels / 20) == 0 || currentPixel == totalPixels) {
                std::cout << "\rProgress: " << std::fixed << std::setprecision(1)
                          << progress << "%";
                std::cout.flush();  // Ensure the output is updated in real-time
            }
        }
    }

    return 0;
}
