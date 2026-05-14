#ifndef EVALUATE_H
#define EVALUATE_H

#include <glm/glm.hpp>
#include "classes/animations/track.h"
#include "classes/animations/easing.h"
#include "external/tinyexpr.h"

// Catmull-Rom: smooth curve through p1→p2, using p0 and p3 as tangent guides.
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

// Evaluates the track at localT ∈ [0, duration].
// The full path is: [capturedStart, waypoint0, waypoint1, ...].
// Easing is applied ONCE globally to t/duration, then that alpha samples the path.
inline glm::vec3 evaluate(Track& track, float localT, float duration) {
    // Global alpha with easing
    float alpha = (duration > 0.0f) ? glm::clamp(localT / duration, 0.0f, 1.0f) : 1.0f;
    alpha = applyEasing(alpha, track.easing);

    // PATH mode: evaluate parametric expressions with t = alpha
    if (track.interpolation == InterpolationMode::PATH) {
        if (track.pathT) *track.pathT = (double)alpha;
        glm::vec3 offset(
            track.pathExprX ? (float)te_eval(track.pathExprX) : 0.0f,
            track.pathExprY ? (float)te_eval(track.pathExprY) : 0.0f,
            track.pathExprZ ? (float)te_eval(track.pathExprZ) : 0.0f
        );
        return track.capturedStart + offset;
    }

    // Build full path: start + waypoints
    const auto& kf = track.keyframes;
    int numPoints = (int)kf.size() + 1;  // capturedStart + waypoints

    if (numPoints == 1) return track.capturedStart;  // no waypoints, stay put

    // Map alpha to path segment
    int segments = numPoints - 1;
    float scaled = alpha * segments;
    int seg = glm::min((int)scaled, segments - 1);
    float segAlpha = scaled - (float)seg;

    // Helper to get point by index in the full path
    auto getPoint = [&](int idx) -> glm::vec3 {
        if (idx == 0) return track.capturedStart;
        return kf[idx - 1].value;
    };

    glm::vec3 p1 = getPoint(seg);
    glm::vec3 p2 = getPoint(seg + 1);

    if (track.interpolation == InterpolationMode::SMOOTH && numPoints >= 2) {
        glm::vec3 p0 = (seg > 0)              ? getPoint(seg - 1) : 2.0f*p1 - p2;
        glm::vec3 p3 = (seg + 2 < numPoints)  ? getPoint(seg + 2) : 2.0f*p2 - p1;
        return catmullRom(p0, p1, p2, p3, segAlpha);
    }

    return glm::mix(p1, p2, segAlpha);
}

#endif
