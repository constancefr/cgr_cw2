#define _USE_MATH_DEFINES
#include <cmath>
#include "camera.h"

Camera::Camera(int imageWidth, int imageHeight, float fieldOfView)
    : width(imageWidth), height(imageHeight), fov(fieldOfView) {
    aspectRatio = static_cast<float>(width) / height;
    scale = tan(fov * 0.5 * M_PI / 180.0); // Convert FOV to radians
}

void Camera::getRayDirection(float u, float v, float& x, float& y, float& z) const {
    // To prevent stretching ?
    // x = (2 * u - 1) * aspectRatio * scale;
    // y = (1 - 2 * v) * aspectRatio * scale;

    // Map (u, v) to normalized device coordinates [-1, 1]
    x = (2 * u - 1) * scale;
    y = (1 - 2 * v) * scale;
    z = -1; // Points into the scene
}
