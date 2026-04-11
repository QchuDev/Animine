#ifndef CURVE_H
#define CURVE_H

#include <functional>
#include <glm/glm.hpp>
#include "classes/entities/entity.h"

class Curve : public IEntity { 
private:
    std::function<glm::vec3(float)> pCurve;
public:
    typedef std::function<glm::vec3(float)> ParamFunction;

    // Create the curve
    Curve(ParamFunction formula, glm::vec3 color, Shader* s); 
    ~Curve();
    void draw(const glm::mat4& view, const glm::mat4& projection) override;
    
};


#endif