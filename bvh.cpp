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

size_t AABB::largest_empty_axis(const std::vector<std::shared_ptr<Shape>>& shapes) const {
    if (shapes.empty()) return 0;

    // Vectors to store the min and max coordinates of each bounding box
    std::vector<double> min_x, max_x, min_y, max_y, min_z, max_z;

    for (const auto& shape : shapes) {
        AABB bbox = shape->get_bbox();
        min_x.push_back(bbox.min.x);
        max_x.push_back(bbox.max.x);
        min_y.push_back(bbox.min.y);
        max_y.push_back(bbox.max.y);
        min_z.push_back(bbox.min.z);
        max_z.push_back(bbox.max.z);
    }

    // Sort the coordinates
    std::sort(min_x.begin(), min_x.end());
    std::sort(max_x.begin(), max_x.end());
    std::sort(min_y.begin(), min_y.end());
    std::sort(max_y.begin(), max_y.end());
    std::sort(min_z.begin(), min_z.end());
    std::sort(max_z.begin(), max_z.end());

    // Calculate the largest gaps between bounding boxes
    double max_gap_x = 0.0, max_gap_y = 0.0, max_gap_z = 0.0;
    for (size_t i = 1; i < shapes.size(); ++i) {
        max_gap_x = std::max(max_gap_x, min_x[i] - max_x[i - 1]);
        max_gap_y = std::max(max_gap_y, min_y[i] - max_y[i - 1]);
        max_gap_z = std::max(max_gap_z, min_z[i] - max_z[i - 1]);
    }

    // Compare the gaps and choose the axis with the largest gap
    if (max_gap_x > max_gap_y && max_gap_x > max_gap_z) return 0;  // X-axis
    if (max_gap_y > max_gap_z) return 1;                           // Y-axis
    return 2;                                                      // Z-axis
}

std::shared_ptr<BVHNode> BVH::build_tree(const std::vector<std::shared_ptr<Shape>>& shapes) {
    // Base case: create a leaf node for 4 or fewer shapes
    if (shapes.size() <= 2) {
        return std::make_shared<BVHNode>(shapes);  // Leaf node with shapes
    }

    // Step 1: Compute the centroid bounding box
    AABB centroid_bbox;
    for (const auto& shape : shapes) {
        centroid_bbox.expand(shape->get_bbox().centroid());
    }

    // Step 2: Determine the axis with the largest extent
    size_t axis = centroid_bbox.largest_empty_axis(shapes);

    // std::cout << "Axis over which we split: " << axis << std::endl;

    // Step 3: Calculate the spatial midpoint along the chosen axis
    double midpoint = 0.5 * (centroid_bbox.min[axis] + centroid_bbox.max[axis]);

    // Step 4: Partition shapes into left and right groups based on the midpoint
    std::vector<std::shared_ptr<Shape>> left_shapes, right_shapes;
    for (const auto& shape : shapes) {
        if (shape->get_bbox().centroid()[axis] < midpoint) {
            left_shapes.push_back(shape);
        } else {
            right_shapes.push_back(shape);
        }
    }

    // Step 5: Handle edge cases (e.g., all shapes on one side of the midpoint)
    if (left_shapes.empty() || right_shapes.empty()) {
        // Fallback to median split
        size_t mid = shapes.size() / 2;
        std::vector<std::shared_ptr<Shape>> sorted_shapes = shapes;
        std::sort(sorted_shapes.begin(), sorted_shapes.end(), [axis](const auto& a, const auto& b) {
            return a->get_bbox().centroid()[axis] < b->get_bbox().centroid()[axis];
        });

        left_shapes = std::vector<std::shared_ptr<Shape>>(sorted_shapes.begin(), sorted_shapes.begin() + mid);
        right_shapes = std::vector<std::shared_ptr<Shape>>(sorted_shapes.begin() + mid, sorted_shapes.end());
    }

    // Step 6: Recursively build the left and right subtrees
    return std::make_shared<BVHNode>(build_tree(left_shapes), build_tree(right_shapes));
}

bool BVH::intersects(const ray& r, double& t_hit, std::shared_ptr<Shape>& hit_shape, double max_t) const {
    // Start at root and recursively check for intersections
    return intersects_node(r, t_hit, hit_shape, max_t, root);
}

bool BVH::intersects_node(const ray& r, double& t_hit, std::shared_ptr<Shape>& hit_shape, double max_t, const std::shared_ptr<BVHNode>& node) const {
    // Check if the ray intersects the bounding box
    if (!node->bbox.intersects(r)) return false;

    if (node->is_leaf()) {
        bool hit = false;
        double closest_t = max_t;
        for (const auto& shape : node->primitives) {
            double t = 0;
            if (shape->intersects(r, t) && t < closest_t && t > 1e-4) {
                t_hit = t;
                closest_t = t;
                hit = true;
                hit_shape = shape;
            }
        }
        return hit;
    }

    // Recursively check the left and right children
    double t_left = max_t, t_right = max_t;
    std::shared_ptr<Shape> shape_left = nullptr, shape_right = nullptr;

    bool hit_left = intersects_node(r, t_left, shape_left, max_t, node->left);
    bool hit_right = intersects_node(r, t_right, shape_right, max_t, node->right);

    // Determine the closest hit between left and right children
    if (hit_left && (!hit_right || t_left < t_right)) {
        t_hit = t_left;
        hit_shape = shape_left;
        return true;
    } else if (hit_right) {
        t_hit = t_right;
        hit_shape = shape_right;
        return true;
    }

    return false;
}
