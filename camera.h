#ifndef CAMERA_H
#define CAMERA_H

#define M_PI 3.14159265358979323846
#include "vector3.h"
#include "ray.h"

class Camera {
public:
    vector3 origin;
    vector3 horizontal;
    vector3 vertical;
    vector3 upper_left_corner;

    Camera(double image_width, double image_height, double fov, const vector3& position, const vector3& lookAt, const vector3& upVector) {
        origin = position;
        double aspect_ratio = image_width / image_height;
        double viewport_height = 2.0 * tan((fov * M_PI / 180.0) / 2.0); // Convert fov to radians
        double viewport_width = aspect_ratio * viewport_height;

        vector3 forward = (lookAt - position).unit();
        vector3 right = upVector.unit().cross(forward);
        vector3 up = forward.cross(right);

        horizontal = right.unit() * viewport_width;
        vertical = up.unit() * viewport_height;
        upper_left_corner = origin + forward - horizontal / 2 + vertical / 2;

    }

    ray get_ray(double u, double v) const {
        return ray(origin, upper_left_corner + horizontal * u - vertical * v - origin);
    }
};

#endif