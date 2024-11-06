#include <iostream>
#include <fstream>

int main() {
    // Image dimensions
    const int width = 200;
    const int height = 100;

    // Open a file in write mode
    std::ofstream imageFile("output.ppm");

    // Write PPM header
    imageFile << "P3\n" << width << " " << height << "\n255\n";

    // Generate the image content
    for (int j = height - 1; j >= 0; --j) {
        for (int i = 0; i < width; ++i) {
            // Generate a color gradient
            float r = static_cast<float>(i) / (width - 1);
            float g = static_cast<float>(j) / (height - 1);
            float b = 0.2f;

            int ir = static_cast<int>(255.99 * r);
            int ig = static_cast<int>(255.99 * g);
            int ib = static_cast<int>(255.99 * b);

            // Write pixel values to the file
            imageFile << ir << " " << ig << " " << ib << "\n";
        }
    }

    // Close the file
    imageFile.close();

    std::cout << "Image generated: output.ppm\n";
    return 0;
}
