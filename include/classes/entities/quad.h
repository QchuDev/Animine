#ifndef QUAD_H
#define QUAD_H

#include <glm/glm.hpp>
#include "classes/entities/entity.h"

class Quad : public IEntity {
private:
    unsigned int VAO, VBO; // The VAO: Vertex Array Object & VBO: Vertex Buffer Object
    glm::vec3 points[4];    // 4 vertices that form the quad
    glm::vec2 uvs[4];       // 4 points of the uv in texture
    unsigned int textureID; // ID of the loaded texture
    glm::vec3 color{1.0f};  // tint color (default white = no tint)
    void setupMesh();
public:
    Quad(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4, unsigned int tex, Shader* s);
    ~Quad();
    void draw(const glm::mat4& view, const glm::mat4& projection) override;
    glm::vec3 getColor() override { return color; }
    void setColor(glm::vec3 c) override { color = c; }
};

#endif