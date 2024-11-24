#ifndef UTILS_H
#define UTILS_H

#include <utility>

double random_double(double min, double max);

std::pair<double, double> normalize_pixel(int i, int j, int width, int height);

#endif
