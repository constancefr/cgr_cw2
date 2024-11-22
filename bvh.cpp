#include "bvh.h"
#include "shape.h"
#include <algorithm>
#include <numeric>

bool AABB::intersects(const ray& r) const {
    // For each of the three axes (x, y, z), check if the ray intersects the slab defined by the bounding box
    double t_min = -INFINITY, t_max = INFINITY;

    for (int i = 0; i < 3; ++i) {        
        double inv_d = 1.0 / r.get_direction()[i]; // Inverse of ray's direction component
        double t0 = (min[i] - r.get_origin()[i]) * inv_d;
        double t1 = (max[i] - r.get_origin()[i]) * inv_d;

        // Ensure t0 is the smaller value
        if (inv_d < 0.0) std::swap(t0, t1);

        // Update the minimum and maximum intersection times
        t_min = std::max(t_min, t0);
        t_max = std::min(t_max, t1);

        // If t_max is smaller than t_min, the ray does not intersect the AABB
        if (t_min > t_max) {
            return false;
        }
        
    }

    return true;
}


BVHNode::BVHNode(const std::vector<std::shared_ptr<Shape>>& shapes) {
    // Calculate the bounding box for all shapes in the list
    for (const auto& shape : shapes) {
        bbox.merge(shape->get_bbox());
    }
    primitives = shapes;
}

BVHNode::BVHNode(std::shared_ptr<BVHNode> left, std::shared_ptr<BVHNode> right)
    : left(left), right(right) {
    bbox = left->bbox;
    bbox.merge(right->bbox);
}

BVH::BVH(const std::vector<std::shared_ptr<Shape>>& shapes) {
    root = build_tree(shapes);
}

std::shared_ptr<BVHNode> BVH::build_tree(const std::vector<std::shared_ptr<Shape>>& shapes) {
    // If there is only one shape, create a leaf node
    if (shapes.size() <= 1) {
        return std::make_shared<BVHNode>(shapes);  // Leaf node with shapes
    }

    // Split shapes into two halves based on a chosen axis (X, Y, Z)
    size_t axis = rand() % 3;  // Random axis, or use other strategies like SAH for efficiency
    std::vector<std::shared_ptr<Shape>> left_shapes, right_shapes;

    // Sort shapes based on the selected axis (e.g., median split)
    std::vector<std::shared_ptr<Shape>> sorted_shapes = shapes;
    std::sort(sorted_shapes.begin(), sorted_shapes.end(), [axis](const std::shared_ptr<Shape>& a, const std::shared_ptr<Shape>& b) {
        return a->get_bbox().min[axis] < b->get_bbox().min[axis];
    });

    size_t mid = sorted_shapes.size() / 2;
    left_shapes = std::vector<std::shared_ptr<Shape>>(sorted_shapes.begin(), sorted_shapes.begin() + mid);
    right_shapes = std::vector<std::shared_ptr<Shape>>(sorted_shapes.begin() + mid, sorted_shapes.end());

    // Recursively build the left and right subtrees
    return std::make_shared<BVHNode>(build_tree(left_shapes), build_tree(right_shapes));
}

bool BVH::intersects(const ray& r, double& t_hit, std::shared_ptr<Shape>& hit_shape, double max_t) const {
    // Perform intersection test with the root BVH node
    return intersects_node(r, t_hit, hit_shape, max_t, root);
}

bool BVH::intersects_node(const ray& r, double& t_hit, std::shared_ptr<Shape>& hit_shape, double max_t, const std::shared_ptr<BVHNode>& node) const {
    // Check if the ray intersects the bounding box
    if (!node->bbox.intersects(r)) return false;

    if (node->is_leaf()) {
        bool hit = false;
        // double closest_t = max_t;
        for (const auto& shape : node->primitives) {
            double t = 0;
            // Check intersection with epsilon (1e-4) and update only if it's closer
            // if (shape->intersects(r, t) && t < closest_t && t > 1e-4) {
            if (shape->intersects(r, t) && t < max_t && t > 1e-4) {
                t_hit = t;
                hit = true;
                hit_shape = shape;
            }
        }
        return hit;
    }

    /*
    if (node->is_leaf()) {
        bool hit = false;
        double closest_t = max_t;
        // double closest_t = t_hit; // Use closest_t to track the nearest intersection
        for (const auto& shape : node->primitives) {
            double t = 0;
            // Check intersection with epsilon (1e-4) and update only if it's closer
            if (shape->intersects(r, t) && t < closest_t && t > 1e-4) {
                closest_t = t;
                hit = true;
                hit_shape = shape;
            }
        }
        t_hit = closest_t; // Update t_hit with the closest intersection
        return hit;
    }
    */

    // Recursively check the left and right children
    bool hit_left = intersects_node(r, t_hit, hit_shape, max_t, node->left);
    bool hit_right = intersects_node(r, t_hit, hit_shape, max_t, node->right);
    return hit_left || hit_right;
}