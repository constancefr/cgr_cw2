#include "tone_mapping.h"
#include <cmath>

vector3 reinhard_tone_mapping(const vector3& color) {
    return color / (color + vector3(1.0, 1.0, 1.0));
}

vector3 exposure_tone_mapping(const vector3& color, float exposure) {
    return vector3(1.0, 1.0, 1.0) - (vector3(-color.x * exposure, -color.y * exposure, -color.z * exposure).exp());
}

vector3 aces_tone_mapping(const vector3& color) {
    const float a = 2.51f;
    const float b = 0.03f;
    const float c = 2.43f;
    const float d = 0.59f;
    const float e = 0.14f;
    return (color * (a * color + b)) / (color * (c * color + d) + e);
}

vector3 gamma_correction(const vector3& color, float gamma) {
    return vector3(pow(color.x, gamma), pow(color.y, gamma), pow(color.z, gamma));
}
