#ifndef SHAPE_H
#define SHAPE_H

#include "ray.h"
#include <memory>
#include <vector>

// Define Material struct that all shapes will share
// struct Material {
//     double ks, kd;                // Specular & diffuse reflection coefficient
//     double specularexponent;
//     vector3 diffusecolor, specularcolor;
//     bool isreflective, isrefractive;
//     double reflectivity, refractiveindex;
// };
struct Material {
    double kd, ks, reflectivity, refractiveindex;
    double transparency = 0.2; // Add transparency
    double specularexponent;
    vector3 diffusecolor, specularcolor;
    bool isreflective, isrefractive; // Flags to enable reflection/refraction
};


// Abstract Shape class
class Shape {
public:
    Material material;

    Shape(const Material& mat) : material(mat) {}

    // Pure virtual method to check for intersection with the ray
    virtual bool intersects(const ray& r, double& t_hit) const = 0;

    virtual vector3 get_normal(const vector3& point) const = 0;

    // Virtual destructor to allow proper cleanup of derived classes
    virtual ~Shape() {}
};

#endif
