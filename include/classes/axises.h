#ifndef AXISES_H
#define AXISES_H

#include <glad/glad.h>
#include "classes/entities/entity.h"

class Axises : public IEntity{
    public:
        Axises();
        void draw(Shader& shader, const glm::mat4& view, const glm::mat4& projection) override;
    private:
        unsigned int VAO, VBO;
        
};

#endif