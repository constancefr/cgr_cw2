#include <iostream>
#include <fstream>
#include "camera.h"

int main() {
    // Image dimensions & field of view for the camera
    const int width = 1920; // 16:9 aspect ratio
    const int height = 1080;
    const float fov = 90.0f;

    // Initialize camera
    Camera camera(width, height, fov);

    // Open a file in write mode & write the PPM header
    std::ofstream imageFile("output.ppm");
    imageFile << "P3\n" << width << " " << height << "\n255\n";

    // Generate the image content
    for (int j = height - 1; j >= 0; --j) {
        for (int i = 0; i < width; ++i) {
            // Convert pixel coordinates to normalized coordinates
            float u = static_cast<float>(i) / (width - 1);
            float v = static_cast<float>(j) / (height - 1);

            // Get ray direction for this pixel
            float rayDirX, rayDirY, rayDirZ;
            camera.getRayDirection(u, v, rayDirX, rayDirY, rayDirZ);

            // Generate a color based on the ray direction - simple gradient (replace later by intersection and shading code)
            int ir = static_cast<int>(255.99 * (rayDirX + 1) * 0.5);
            int ig = static_cast<int>(255.99 * (rayDirY + 1) * 0.5);
            int ib = static_cast<int>(255.99 * (rayDirZ + 1) * 0.5);
            // int ir = static_cast<int>(255.99 * u); // Red channel varies horizontally
            // int ig = static_cast<int>(255.99 * v); // Green channel varies vertically
            // int ib = 128; // Constant value for blue channel

            // Write pixel values to the file
            imageFile << ir << " " << ig << " " << ib << "\n";
        }
    }

    // Close the file
    imageFile.close();

    std::cout << "Image generated with camera view: output.ppm\n";
    return 0;
}
