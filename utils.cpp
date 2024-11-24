#include "utils.h"
#include <random>

// Define the random_double function
double random_double(double min, double max) {
    static std::random_device rd;   // only one instance
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(min, max);
    return dis(gen);
}

// Center normalised coordinates inside pixel
std::pair<double, double> normalize_pixel(int i, int j, int width, int height) {
    return { (i + 0.5) / width, (j + 0.5) / height }; // Center pixel by default
}