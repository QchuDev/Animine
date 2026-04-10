#ifndef CURVE_H
#define CURVE_H

#include <functional>
#include <glm/glm.hpp>

class Curve { 
private:
    std::function<glm::vec3(float)> pCurve;
public:
    typedef std::function<glm::vec3(float)> ParamFunction;
    
    // Create the curve
    Curve(ParamFunction formula); 
    ~Curve();
    
};


#endif