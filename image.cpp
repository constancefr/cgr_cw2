#include <iostream>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <cmath>

#include "image.h"
#include "vector3.h"

Image::Image(const std::string& file_path) {
    load_image(file_path);
}

// Loads a BMP image from the given file path
void Image::load_image(const std::string& file_path) {
    std::ifstream file(file_path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open file: " + file_path);
    }

    // Read the BMP file header
    char header[54];
    file.read(header, 54);
    
    if (header[0] != 'B' || header[1] != 'M') {
        throw std::runtime_error("Not a BMP file: " + file_path);
    }

    // Extract image dimensions from the header
    width = *reinterpret_cast<int*>(&header[18]);
    height = *reinterpret_cast<int*>(&header[22]);
    channels = 3;  // BMP images are usually 24-bit (RGB)

    // Calculate the row size (padded to 4 bytes)
    int row_size = (width * channels + 3) & ~3;  // Align rows to 4-byte boundaries

    // Read pixel data
    data.resize(row_size * height);
    file.seekg(*reinterpret_cast<int*>(&header[10]), std::ios::beg);  // Jump to the pixel data
    file.read(reinterpret_cast<char*>(data.data()), row_size * height);

    file.close();
}

// Returns the color of the pixel at the given UV coordinates on the image (used for texture mapping)
vector3 Image::get_color_at_uv(double u, double v) const {
    // Clamp UV coordinates
    u = std::fmod(u, 1.0);
    if (u < 0) u += 1.0;
    v = std::fmod(v, 1.0);
    if (v < 0) v += 1.0;

    // Convert UV to pixel coordinates
    int x = static_cast<int>(u * width);
    int y = static_cast<int>((1.0 - v) * height);  // Flip y-axis to match image orientation

    // Retrieve color at (x, y)
    int index = (y * ((width * channels + 3) & ~3)) + (x * channels);  // Account for padding
    return vector3(
        data[index + 2] / 255.0,   // R
        data[index + 1] / 255.0,   // G
        data[index] / 255.0        // B
    );
}