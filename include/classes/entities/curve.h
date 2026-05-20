#ifndef CURVE_H
#define CURVE_H

#include <functional>
#include <glm/glm.hpp>
#include "classes/entities/entity.h"

class Curve : public IEntity { 
private:
    std::function<glm::vec3(float)> pCurve;
    unsigned int VAO, VBO;
    unsigned int textureID;
    glm::vec3 color;
    int m_vertexCount = 0;
    float width = 0.04f;

    std::vector<glm::vec3> points;
    void buildStrip(const glm::vec3& camPos);

public:
    typedef std::function<glm::vec3(float)> ParamFunction;

    Curve(ParamFunction formula, glm::vec3 color, Shader* s, unsigned int strokeTex, float tMin = 0.0f, float tMax = 6.2832f);
    ~Curve();
    void draw(const glm::mat4& view, const glm::mat4& projection) override;
    glm::vec3 getColor() override { return color; }
    void setColor(glm::vec3 c) override { color = c; }
};

#endif
