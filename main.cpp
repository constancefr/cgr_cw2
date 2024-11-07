#include <iostream>
#include <fstream>
#include <iomanip>
#include "camera.h"
#include "sphere.h"
#include "vector3.h"

int main() {
    // Image dimensions & field of view for the camera
    const int width = 1920; // 16:9 aspect ratio
    const int height = 1080;
    const int totalPixels = width * height;
    const float fov = 90.0f;

    // Initialize camera
    Camera camera(width, height, fov);

    // Create a sphere at (0, 0, -1) with radius 0.5
    Sphere sphere(Vector3(0.4, 0.4, -1), 0.2f);

    // Open a file in write mode & write the PPM header
    std::ofstream imageFile("output.ppm");
    imageFile << "P3\n" << width << " " << height << "\n255\n";

    std::cout << "Rendering started...\n";

    // Generate the image content
    for (int j = 0; j < height; ++j) {
        for (int i = 0; i < width; ++i) {
            // Convert pixel coordinates to normalized coordinates
            float u = static_cast<float>(i) / (width - 1);
            float v = static_cast<float>(j) / (height - 1);

            // Get ray direction for this pixel
            float rayDirX, rayDirY, rayDirZ;
            camera.getRayDirection(u, v, rayDirX, rayDirY, rayDirZ);

            // Create a ray starting at (0, 0, 0)
            Vector3 rayOrigin(0, 0, 0);
            Vector3 rayDir(rayDirX, rayDirY, rayDirZ);
            rayDir = rayDir.normalize();

            // Check for intersection with the sphere
            float t1, t2;
            if (sphere.intersect(rayOrigin, rayDir, t1, t2)) {
                // If there is an intersection, color the pixel (simple coloring)
                imageFile << "255 0 0\n";  // Red color
            } else {
                // No intersection, color the pixel as background (sky)
                imageFile << "0 0 255\n";  // Blue color
            }

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

    // Close the file
    imageFile.close();

    std::cout << "\nImage generated with camera view & sphere intersection: output.ppm\n";
    return 0;
}
