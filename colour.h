#ifndef COLOUR_H
#define COLOUR_H

#include <iostream>
#include "vector3.h"

void write_colour(std::ostream& out, const vector3& colour) {
    out << static_cast<int>(255.999 * colour.x) << " "
        << static_cast<int>(255.999 * colour.y) << " "
        << static_cast<int>(255.999 * colour.z) << "\n";
}

#endif
