#ifndef VECTOR3_H
#define VECTOR3_H
#include <cmath>

class Vector3 {
public:
    float x, y, z;

    Vector3() : x(0), y(0), z(0) {}
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

    // Vector subtraction
    Vector3 operator-(const Vector3& other) const {
        return Vector3(x - other.x, y - other.y, z - other.z);
    }

    // Dot product
    float dot(const Vector3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    // Scalar multiplication
    Vector3 operator*(float scalar) const {
        return Vector3(x * scalar, y * scalar, z * scalar);
    }

    Vector3 normalize() const {
    float len = sqrt(x * x + y * y + z * z);
    return Vector3(x / len, y / len, z / len);
    }
};

#endif
