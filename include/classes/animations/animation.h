#ifndef ANIMATION_H
#define ANIMATION_H

#include <string>
#include <vector>
#include <glm/glm.hpp>
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

class InstantSet : public IAnimation {
public:
    float startTime = 0.0f;
    std::string entity_id;
    TransformProp property;
    glm::vec3 value;
    bool applied = false;
};

#endif
