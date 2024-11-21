#ifndef vector3_H
#define vector3_H

#include <cmath>
#include <iostream>

class vector3 {
public:
    double x, y, z;

    vector3() : x(0), y(0), z(0) {}
    vector3(double x, double y, double z) : x(x), y(y), z(z) {}

    // Basic operations
    vector3 operator+(const vector3& v) const { 
        return vector3(x + v.x, y + v.y, z + v.z);
    }
    vector3 operator+(double scalar) const {
        return vector3(x + scalar, y + scalar, z + scalar);
    }
    friend vector3 operator+(double scalar, const vector3& v) {
        return vector3(v.x + scalar, v.y + scalar, v.z + scalar);
    }
    vector3& operator+=(const vector3& v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this; 
    }
    vector3 operator-(const vector3& v) const { // subtraction
        return vector3(x - v.x, y - v.y, z - v.z); 
    }
    vector3 operator-() const { // negation
        return vector3(-x, -y, -z);
    }
    vector3 operator*(double scalar) const {  // scalar multiplication
        return vector3(x * scalar, y * scalar, z * scalar); 
    }
    friend vector3 operator*(double scalar, const vector3& v) { // commutative scalar multiplication
        return v * scalar; 
    }
    vector3 operator*(const vector3& v) const { // element-wise multiplication
        return vector3(x * v.x, y * v.y, z * v.z); 
    }

    vector3 operator/(double scalar) const { 
        return *this * (1 / scalar); 
    }
    vector3 operator/(const vector3& v) const { // element-wise division
        return vector3(x / v.x, y / v.y, z / v.z);
    }
    vector3 exp() const { // element-wise exponentiation
        return vector3(std::exp(x), std::exp(y), std::exp(z));
    }

    // Dot and cross product
    double dot(const vector3& v) const { 
        return x * v.x + y * v.y + z * v.z;
    }
    vector3 cross(const vector3& v) const { 
        return vector3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); 
    }

    // Utility
    double length() const { 
        return std::sqrt(x * x + y * y + z * z); 
    }
    vector3 unit() const { 
        return *this / length(); 
    }
    friend std::ostream& operator<<(std::ostream& os, const vector3& v) {
        return os << v.x << " " << v.y << " " << v.z;
    }
};

#endif
