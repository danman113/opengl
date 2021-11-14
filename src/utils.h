#pragma once

#include <algorithm>

namespace Math {
    float lerp(float val, float inMin, float inMax);

    float map(float val, float inMin, float inMax, float outMin, float outMax);
}
