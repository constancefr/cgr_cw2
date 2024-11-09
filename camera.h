#ifndef CAMERA_H
#define CAMERA_H

#include "vector3.h"
#include "ray.h"

class Camera {
public:
    vector3 origin;
    vector3 horizontal;
    vector3 vertical;
    vector3 upper_left_corner;

    Camera(double aspect_ratio, double viewport_height, double focal_length) {
        double viewport_width = aspect_ratio * viewport_height;

        origin = vector3(0, 0, 0);
        horizontal = vector3(viewport_width, 0, 0);
        vertical = vector3(0, -viewport_height, 0);
        upper_left_corner = origin - horizontal / 2 - vertical / 2 - vector3(0, 0, focal_length);
    }

    ray get_ray(double u, double v) const {
        return ray(origin, upper_left_corner + horizontal * u + vertical * v - origin);
    }
};

#endif
