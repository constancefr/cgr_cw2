#include "image.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Image::Image(const std::string& file_path) {
    load_image(file_path);
}

void Image::load_image(const std::string& file_path) {
    unsigned char* img_data = stbi_load(file_path.c_str(), &width, &height, &channels, 3);  // Force RGB
    if (!img_data) {
        throw std::runtime_error("Failed to load texture: " + file_path);
    }

    data.assign(img_data, img_data + width * height * 3);  // Copy data
    stbi_image_free(img_data);  // Free memory allocated by stb_image
}

vector3 Image::get_color_at_uv(double u, double v) const {
    // Clamp UV coordinates
    u = std::fmod(u, 1.0);
    if (u < 0) u += 1.0;
    v = std::fmod(v, 1.0);
    if (v < 0) v += 1.0;

    // Convert UV to pixel coordinates
    int x = static_cast<int>(u * width);
    int y = static_cast<int>(v * height);

    // Retrieve color at (x, y)
    int index = (y * width + x) * 3;
    return vector3(
        data[index] / 255.0,     // R
        data[index + 1] / 255.0, // G
        data[index + 2] / 255.0  // B
    );
}