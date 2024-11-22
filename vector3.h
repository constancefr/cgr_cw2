#ifndef vector3_H
#define vector3_H
#define M_PI 3.14159265358979323846

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
    // Accessor method for individual components
    double operator[](int i) const {
        if (i == 0) return x;
        else if (i == 1) return y;
        else if (i == 2) return z;
        else throw std::out_of_range("Index out of range in vector3");
    }

    vector3 random_perturbation(const vector3& normal, double spread) const {
        // Generate a random direction within a cone of angle `spread`
        vector3 tangent1, tangent2;
        orthonormal_basis(normal, tangent1, tangent2);

        double phi = 2.0 * M_PI * random_double(); // Random angle around the cone
        double z = std::cos(spread * random_double()); // Random height in the cone
        double xy = std::sqrt(1.0 - z * z);

        // Convert spherical coordinates to Cartesian
        vector3 random_dir = z * normal + xy * std::cos(phi) * tangent1 + xy * std::sin(phi) * tangent2;
        return random_dir.unit(); // Normalize
    }

    void orthonormal_basis(const vector3& n, vector3& t1, vector3& t2) const {
        if (std::fabs(n.x) > std::fabs(n.z)) {
            t1 = vector3(-n.y, n.x, 0).unit();
        } else {
            t1 = vector3(0, -n.z, n.y).unit();
        }
        t2 = n.cross(t1).unit();
    }

    double random_double() const {
        return rand() / (RAND_MAX + 1.0);
    }
};

#endif
