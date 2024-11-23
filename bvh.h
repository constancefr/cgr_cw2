#ifndef BVH_H
#define BVH_H

#include "shape.h"
#include "vector3.h"
#include "ray.h"
#include <memory>
#include <vector>
#include <algorithm>

class Shape;

// Axis-aligned bounding box (bounding volume)
struct AABB {
    vector3 min, max;

    AABB() : min(vector3(INFINITY, INFINITY, INFINITY)), max(vector3(-INFINITY, -INFINITY, -INFINITY)) {}

    // Merge two AABBs to form a larger one that contains both
    void merge(const AABB& other) {
        min = vector3(std::min(min.x, other.min.x), std::min(min.y, other.min.y), std::min(min.z, other.min.z));
        max = vector3(std::max(max.x, other.max.x), std::max(max.y, other.max.y), std::max(max.z, other.max.z));
    }

    // // Calculate surface area of the AABB
    // double surface_area() const {
    //     vector3 extent = max - min; // Calculate width, height, and depth
    //     return 2.0 * (extent.x * extent.y + extent.x * extent.z + extent.y * extent.z);
    // }

    // Expand the AABB to include a point
    void expand(const vector3& point) {
        min = vector3(std::min(min.x, point.x), std::min(min.y, point.y), std::min(min.z, point.z));
        max = vector3(std::max(max.x, point.x), std::max(max.y, point.y), std::max(max.z, point.z));
    }

    // Calculate the centroid of the AABB
    vector3 centroid() const {
        return 0.5 * (min + max);
    }

    // Calculate the extent of the AABB (size along each axis)
    vector3 extent() const {
        return max - min;
    }

    size_t largest_empty_axis(const std::vector<std::shared_ptr<Shape>>& shapes) const;

    /* Does not take empty space into account */
    /*
    size_t largest_extent_axis() const {
        vector3 e = extent();
        if (e.x > e.y && e.x > e.z) return 0;  // X-axis
        if (e.y > e.z) return 1;               // Y-axis
        return 2;                              // Z-axis
    }
    */
    
    // Check if a ray intersects this AABB
    bool intersects(const ray& r) const;
};


class BVHNode {
public:
    AABB bbox;  // Bounding box for this node
    std::shared_ptr<BVHNode> left, right;  // Child nodes
    std::vector<std::shared_ptr<Shape>> primitives;  // List of shapes at this node (leaf node)

    // Constructor for leaf nodes
    BVHNode(const std::vector<std::shared_ptr<Shape>>& shapes);

    // Constructor for internal nodes
    BVHNode(std::shared_ptr<BVHNode> left, std::shared_ptr<BVHNode> right);

    bool is_leaf() const { return primitives.size() > 0; }
};

class BVH {
public:
    std::shared_ptr<BVHNode> root;

    BVH(const std::vector<std::shared_ptr<Shape>>& shapes);

    bool intersects(const ray& r, double& t_hit, std::shared_ptr<Shape>& hit_shape, double max_t) const;

    bool intersects_node(const ray& r, double& t_hit, std::shared_ptr<Shape>& hit_shape, double max_t, const std::shared_ptr<BVHNode>& node) const;
    
private:
    std::shared_ptr<BVHNode> build_tree(const std::vector<std::shared_ptr<Shape>>& shapes);
};

#endif
