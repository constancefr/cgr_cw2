#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include "libs/json.hpp"
#include "vector3.h"

using json = nlohmann::json;

struct Shape {
    std::string type;
    vector3 center;
    double radius;
    vector3 axis;
    double height;
    std::vector<vector3> vertices; // For triangles
};

class Scene {
public:
    vector3 backgroundcolor;
    // std::vector<Shape> shapes;

    Scene(const vector3& background_color) {
        backgroundcolor = background_color;
    }
    
};

#endif

    // Scene(const json& scene_json) {
    //     background_color = vector3(scene_json["backgroundcolor"][0],
    //                              scene_json["backgroundcolor"][1],
    //                              scene_json["backgroundcolor"][2]);

    //     for (const auto& shape_json : scene_json["shapes"]) {
    //         Shape shape;
    //         shape.type = shape_json["type"];
    //         shape.center = vector3(shape_json["center"][0],
    //                              shape_json["center"][1],
    //                              shape_json["center"][2]);
    //         if (shape.type == "sphere") {
    //             shape.radius = shape_json["radius"];
    //         } else if (shape.type == "cylinder") {
    //             shape.axis = vector3(shape_json["axis"][0],
    //                                shape_json["axis"][1],
    //                                shape_json["axis"][2]);
    //             shape.radius = shape_json["radius"];
    //             shape.height = shape_json["height"];
    //         } else if (shape.type == "triangle") {
    //             shape.vertices.emplace_back(shape_json["v0"][0], shape_json["v0"][1], shape_json["v0"][2]);
    //             shape.vertices.emplace_back(shape_json["v1"][0], shape_json["v1"][1], shape_json["v1"][2]);
    //             shape.vertices.emplace_back(shape_json["v2"][0], shape_json["v2"][1], shape_json["v2"][2]);
    //         }
    //         shapes.push_back(shape);
    //     }
    // }

