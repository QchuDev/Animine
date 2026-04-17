#ifndef TRANSFORMATION_H
#define TRANSFORMATION_H
#include <iostream>
#include <glm/glm.hpp>

class Transformation {
    public:
            
    private:
        std::string id;
        std::string transform_prop;
        std::string transition_type;
        float currrent_time;
        float duration;
        glm::vec3 target;

};

#endif