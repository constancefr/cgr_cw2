#ifndef TONE_MAPPING_H
#define TONE_MAPPING_H

#include "vector3.h"

vector3 reinhard_tone_mapping(const vector3& color);
vector3 exposure_tone_mapping(const vector3& color, float exposure);
vector3 aces_tone_mapping(const vector3& color);
vector3 gamma_correction(const vector3& color, float gamma);

#endif
