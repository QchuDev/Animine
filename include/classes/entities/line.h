#ifndef LINE_H
#define LINE_H

#include <glm/glm.hpp>
#include "classes/entities/entity.h"

class Line : public IEntity {
public:
    Line(glm::vec3 startPos, glm::vec3 endPos, glm::vec3 color, Shader* s, unsigned int strokeTex);
    ~Line();
    void draw(const glm::mat4& view, const glm::mat4& projection) override;
    glm::vec3 getColor() override { return color; }
    void setColor(glm::vec3 c) override { color = c; }

private:
    unsigned int VAO, VBO;
    unsigned int textureID;
    glm::vec3 color;
    glm::vec3 startPos, endPos;
    float width = 0.05f;

    void buildQuad(const glm::vec3& camPos);
};

#endif
