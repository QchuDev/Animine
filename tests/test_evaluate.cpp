#include <cassert>
#include <cmath>
#include <cstdio>
#include <glm/glm.hpp>
#include "classes/animations/track.h"
#include "classes/animations/evaluate.h"

static bool near(float a, float b) { return std::fabs(a - b) < 1e-4f; }
static bool nearV(glm::vec3 a, glm::vec3 b) { return near(a.x,b.x) && near(a.y,b.y) && near(a.z,b.z); }

int main() {
    Track t;
    t.easing        = EasingType::LINEAR;
    t.interpolation = InterpolationMode::LINEAR;
    t.keyframes     = { {0.0f, {0,0,0}}, {1.0f, {2,4,0}}, {2.0f, {2,4,6}} };

    // Clamp before start
    assert(nearV(evaluate(t, -1.0f), {0,0,0}));
    // Clamp after end
    assert(nearV(evaluate(t, 99.0f), {2,4,6}));
    // Midpoint of first segment
    assert(nearV(evaluate(t, 0.5f),  {1,2,0}));
    // Midpoint of second segment
    assert(nearV(evaluate(t, 1.5f),  {2,4,3}));
    // Exact keyframe
    assert(nearV(evaluate(t, 1.0f),  {2,4,0}));

    // Smooth mode: with 2 keyframes must not crash and endpoints must be exact
    t.interpolation = InterpolationMode::SMOOTH;
    t.keyframes     = { {0.0f, {0,0,0}}, {1.0f, {1,1,0}} };
    assert(nearV(evaluate(t, 0.0f), {0,0,0}));
    assert(nearV(evaluate(t, 1.0f), {1,1,0}));

    printf("evaluate: all tests passed\n");
    return 0;
}
