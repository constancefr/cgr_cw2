#ifndef RAY_H
#define RAY_H

#include "vector3.h"

class ray {
public:
    vector3 origin;
    vector3 direction;
    vector3 inv_direction; // Inverse direction: 1/direction
    int sign[3];           // Sign of each component of the direction
    
    ray(const vector3& o, const vector3& d) : origin(o), direction(d) {
        // Compute inv_direction and sign
        inv_direction = vector3(1.0 / direction.x, 1.0 / direction.y, 1.0 / direction.z);
        for (int i = 0; i < 3; ++i) {
            sign[i] = (inv_direction[i] < 0); // 0 if positive, 1 if negative
        }
    }

    vector3 at(double t) const { return origin + direction * t; }

    vector3 get_direction() const { return direction; }

    vector3 get_origin() const { return origin; }

    // vector3 calc_inv_direction() const {
    //     return vector3(1.0 / direction.x, 1.0 / direction.y, 1.0 / direction.z);
    // }

    vector3 get_inv_direction() const { return inv_direction; }
        
    const int* get_sign() const { return sign; }
};

#endif
