#ifndef EASING_H
#define EASING_H

#include <cmath>
#include "classes/animations/easing_type.h"

inline float applyEasing(float a, EasingType type) {
    switch (type) {
        case EasingType::EASE_IN:      return a * a;
        case EasingType::EASE_OUT:     return 1.0f - (1.0f - a) * (1.0f - a);
        case EasingType::EASE_IN_OUT:  return a < 0.5f ? 2.0f*a*a : 1.0f - 2.0f*(1.0f-a)*(1.0f-a);
        default:                       return a;  // LINEAR
    }
}

#endif
