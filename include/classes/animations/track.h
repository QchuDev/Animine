#ifndef TRACK_H
#define TRACK_H

#include <string>
#include <vector>
#include "classes/animations/easing_type.h"
#include "classes/animations/keyframe.h"

enum class TransformProp     { POSITION, ROTATION, SCALE };
enum class InterpolationMode { LINEAR, SMOOTH };  // SMOOTH = Catmull-Rom

struct Track {
    std::string      entity_id;
    TransformProp    property;
    EasingType       easing;
    InterpolationMode interpolation = InterpolationMode::LINEAR;
    std::vector<Keyframe> keyframes;
};

#endif
