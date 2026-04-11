#include "classes/curve.h"
#include <glm/glm.hpp>

Curve::Curve(ParamFunction formula, float duration) : pCurve(formula), m_duration(duration) {}

// Evaluate the t -> ...
glm::vec3 Curve::evaluate(float t) const { 
    float normalizedT = fmod(t/m_duration, 1.0f); 
    return pCurve(normalizedT);
}