#ifndef CURVE_H
#define CURVE_H

#include <functional>
#include <glm/glm.hpp>
#include "classes/entities/entity.h"

class Curve : public IEntity{ 
private:
    std::function<glm::vec3(float)> pCurve;
    float m_duration;

public:
    typedef std::function<glm::vec3(float)> ParamFunction;
    
    // Create/Delete the curve
    Curve(ParamFunction formula, float duration = 1.0f); 
    ~Curve();
    
    // draw
    void draw(const glm::mat4& view, const glm::mat4& projection) override;
    
    // evaluate the 
    glm::vec3 evaluate(float t) const;
    
    // Change the formula
    void setFormula(ParamFunction new_function) { pCurve = new_function; }  
    
};

#endif