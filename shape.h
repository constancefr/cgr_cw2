#ifndef SHAPE_H
#define SHAPE_H

#include "ray.h"
#include "image.h"
#include "bvh.h"
#include <memory>
#include <vector>
#include <optional>

struct AABB;

struct Material {
    double kd, ks, reflectivity, refractiveindex;
    double transparency = 0.5;
    double specularexponent;
    vector3 diffusecolor, specularcolor;
    bool isreflective, isrefractive;
    
    // Add texture information
    std::string texture_file;  // Path to texture file
    std::shared_ptr<Image> texture;  // Pointer to loaded texture data

    // Default material constructor
    Material() : kd(0.0), ks(0.0), reflectivity(0.0), refractiveindex(1.0), specularexponent(0.0),
                 diffusecolor(0.0, 0.0, 0.0), specularcolor(0.0, 0.0, 0.0), isreflective(false), isrefractive(false) {}
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

    // Texture support
    virtual std::pair<double, double> get_uv(const vector3& point) const = 0;

    // Get the bounding box of the shape
    virtual AABB get_bbox() const = 0;
    
};

#endif
