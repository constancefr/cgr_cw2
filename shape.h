#ifndef SHAPE_H
#define SHAPE_H

#include "ray.h"
#include <memory>
#include <vector>

// Define Material struct that all shapes will share
struct Material {
    double ks;                // Specular reflection coefficient
    double kd;                // Diffuse reflection coefficient
    double specularexponent;  // Specular exponent (shininess)
    vector3 diffusecolor;     // Color for diffuse reflection
    vector3 specularcolor;    // Color for specular reflection
    bool isreflective;        // Whether the material is reflective
    bool isrefractive;        // Whether the material is refractive
    double reflectivity;      // Reflectivity coefficient
    double refractiveindex;   // Refractive index
};

// Abstract Shape class
class Shape {
public:
    Material material;

    Shape(const Material& mat) : material(mat) {}

    // Pure virtual method to check for intersection with the ray
    virtual bool intersects(const ray& r, double& t_hit) const = 0;

    // Virtual destructor to allow proper cleanup of derived classes
    virtual ~Shape() {}
};

#endif
