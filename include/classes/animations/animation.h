#ifndef ANIMATION_H
#define ANIMATION_H

#include <vector>
#include "classes/animations/track.h"

class IAnimation {
public:
    virtual ~IAnimation() = default;
};

class Animation : public IAnimation {
public:
    float startTime = 0.0f;
    float duration  = 0.0f;
    std::vector<Track> tracks;
};

#endif
