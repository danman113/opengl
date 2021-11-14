#include "utils.h"

namespace Math
{
    float lerp(float val, float inMin, float inMax) {
        return ((val - inMin) / (inMax - inMin)); 
    }

    float map(float val, float inMin, float inMax, float outMin, float outMax) {
        return ((val - inMin) / (inMax - inMin)) * (outMax - outMin)  + outMin; 
    }
}
