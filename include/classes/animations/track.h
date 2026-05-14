#ifndef TRACK_H
#define TRACK_H

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "classes/animations/easing_type.h"
#include "classes/animations/keyframe.h"

// Forward-declare te_expr to avoid pulling tinyexpr.h into every TU
extern "C" {
    struct te_expr;
    void te_free(te_expr* n);
}

enum class TransformProp     { POSITION, ROTATION, SCALE };
enum class InterpolationMode { LINEAR, SMOOTH, PATH };  // PATH = parametric x(t) y(t) z(t)

struct Track {
    std::string      entity_id;
    TransformProp    property;
    EasingType       easing;
    InterpolationMode interpolation = InterpolationMode::LINEAR;
    std::vector<Keyframe> keyframes;  // waypoints only (no start value)

    // PATH mode: parametric expressions evaluated with t ∈ [0,1]
    te_expr* pathExprX = nullptr;
    te_expr* pathExprY = nullptr;
    te_expr* pathExprZ = nullptr;
    double*  pathT     = nullptr;  // heap-allocated so te_expr pointers survive moves

    // Captured at runtime on first frame of animation
    glm::vec3 capturedStart{0.0f};
    bool      hasCaptured = false;

    ~Track() {
        te_free(pathExprX);
        te_free(pathExprY);
        te_free(pathExprZ);
        delete pathT;
    }

    // Disable copy (te_expr* ownership), enable move
    Track() = default;
    Track(const Track&) = delete;
    Track& operator=(const Track&) = delete;
    Track(Track&& o) noexcept
        : entity_id(std::move(o.entity_id)), property(o.property), easing(o.easing),
          interpolation(o.interpolation), keyframes(std::move(o.keyframes)),
          pathExprX(o.pathExprX), pathExprY(o.pathExprY), pathExprZ(o.pathExprZ),
          pathT(o.pathT), capturedStart(o.capturedStart), hasCaptured(o.hasCaptured)
    { o.pathExprX = o.pathExprY = o.pathExprZ = nullptr; o.pathT = nullptr; }
    Track& operator=(Track&& o) noexcept {
        if (this != &o) {
            te_free(pathExprX); te_free(pathExprY); te_free(pathExprZ); delete pathT;
            entity_id = std::move(o.entity_id); property = o.property;
            easing = o.easing; interpolation = o.interpolation;
            keyframes = std::move(o.keyframes);
            pathExprX = o.pathExprX; pathExprY = o.pathExprY; pathExprZ = o.pathExprZ;
            pathT = o.pathT; capturedStart = o.capturedStart; hasCaptured = o.hasCaptured;
            o.pathExprX = o.pathExprY = o.pathExprZ = nullptr; o.pathT = nullptr;
        }
        return *this;
    }
};

#endif
