#ifndef LINE_H
#define LINE_H

#include <glm/glm.hpp>
#include "classes/entities/entity.h"

class Line : public IEntity{
public:
    Line(glm::vec3 startPos, glm::vec3 endPos, glm::vec3 color, Shader* s);
    ~Line();
    void draw(const glm::mat4& view, const glm::mat4& projection) override;
private:
    unsigned int VAO, VBO;
};

#endif