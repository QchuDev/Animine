#ifndef AXISES_H
#define AXISES_H

#include <glm/glm.hpp>
#include <glad/glad.h>
#include "classes/entities/entity.h"

class Axises : public IEntity{
    public:
        Axises(Shader* s);
        ~Axises();
        // fix now
        void draw(const glm::mat4& view, const glm::mat4& projection) override;
    private:
        unsigned int VAO, VBO;
        
};

#endif