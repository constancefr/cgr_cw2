#ifndef CAMERA_H
#define CAMERA_H

#include <cmath>

class Camera {
public:
    Camera(int imageWidth, int imageHeight, float fieldOfView);

    // Generate ray direction for each pixel
    void getRayDirection(float u, float v, float& x, float& y, float& z) const;

private:
    int width, height;
    float fov;
    float aspectRatio;
    float scale;
};

#endif
