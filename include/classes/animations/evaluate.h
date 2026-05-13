#ifndef EVALUATE_H
#define EVALUATE_H

#include <glm/glm.hpp>
#include "classes/animations/track.h"
#include "classes/animations/easing.h"

// Catmull-Rom: smooth curve through p1→p2, using p0 and p3 as tangent guides.
// alpha ∈ [0,1] is the local parameter between p1 and p2.
inline glm::vec3 catmullRom(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float alpha) {
    float a2 = alpha * alpha;
    float a3 = a2 * alpha;
    return 0.5f * (
        (2.0f * p1) +
        (-p0 + p2) * alpha +
        (2.0f*p0 - 5.0f*p1 + 4.0f*p2 - p3) * a2 +
        (-p0 + 3.0f*p1 - 3.0f*p2 + p3) * a3
    );
}

// Returns the interpolated value for a track at localT (time relative to animation start).
inline glm::vec3 evaluate(const Track& track, float localT) {
    const auto& kf = track.keyframes;
    if (kf.empty()) return glm::vec3(0.0f);
    if (kf.size() == 1 || localT <= kf.front().time) return kf.front().value;
    if (localT >= kf.back().time)                     return kf.back().value;

    // Find segment: kf[i] <= localT < kf[i+1]
    int i = 0;
    for (int n = (int)kf.size() - 1; i < n - 1 && kf[i+1].time <= localT; ++i);

    float segDuration = kf[i+1].time - kf[i].time;
    float alpha = (localT - kf[i].time) / segDuration;
    alpha = applyEasing(alpha, track.easing);

    if (track.interpolation == InterpolationMode::SMOOTH && kf.size() >= 2) {
        // Mirror endpoints to synthesize missing neighbors
        glm::vec3 p0 = (i > 0)                    ? kf[i-1].value : 2.0f*kf[i].value   - kf[i+1].value;
        glm::vec3 p1 = kf[i].value;
        glm::vec3 p2 = kf[i+1].value;
        glm::vec3 p3 = (i+2 < (int)kf.size())     ? kf[i+2].value : 2.0f*kf[i+1].value - kf[i].value;
        return catmullRom(p0, p1, p2, p3, alpha);
    }

    return glm::mix(kf[i].value, kf[i+1].value, alpha);
}

#endif
