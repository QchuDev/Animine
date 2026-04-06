#ifndef LINE_H
#define LINE_H

#include <glm/glm.hpp>
#include "classes/entities/line.h"
#include "classes/entities/entity.h"

class Line : public IEntity{
public:
    Line(glm::vec3 startPos, glm::vec3 endPos, glm::vec3 color);
    ~Line();
    void draw(Shader& shader, const glm::mat4& view, const glm::mat4& projection) override;
};

#endif