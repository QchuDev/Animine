#include <glm/glm.hpp>
#include "classes/entities/curve.h"


/**
 * Creation of the curve entity. We specify the formula to use, color and shader.
 */
Curve::Curve(ParamFunction formula, glm::vec3 color, Shader* s) 
: IEntity(s) 
{

}

void Curve::draw(const glm::mat4& view, const glm::mat4& projection) {

}


Curve::~Curve() {}
