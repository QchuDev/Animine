#ifndef TRANSFORMATION_H
#define TRANSFORMATION_H
#include <iostream>
#include <glm/glm.hpp>
#include "classes/animations/animation.h"

class Transformation : public IAnimation {
    public:
                    
    private:
        std::string entity_id;
        std::string transform_prop;
        std::string transition_type;
        float currrent_time;
        float duration;
        glm::vec3 target;

};

#endif