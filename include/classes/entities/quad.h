#ifndef QUAD_H
#define QUAD_H

#include <glm/glm.hpp>
#include "classes/entities/entity.h"

class Quad : public IEntity {
public:
    Quad();
    ~Quad();
    void draw(Shader& shader, const glm::mat4& view, const glm::mat4& projection) override;
private:
    unsigned int VAO, VBO;
};

#endif