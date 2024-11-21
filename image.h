#ifndef IMAGE_H
#define IMAGE_H

#include <string>
#include <vector>
#include "vector3.h"

class Image {
public:
    int width, height, channels;
    std::vector<unsigned char> data;  // Raw pixel data (RGB or RGBA)

    Image(const std::string& file_path);
    vector3 get_color_at_uv(double u, double v) const;

private:
    void load_image(const std::string& file_path);
};

#endif
