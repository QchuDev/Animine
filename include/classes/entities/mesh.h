#ifndef MESH_H
#define MESH_H

#include "classes/entities/entity.h"
#include <string>

class Mesh : public IEntity {
public:
    Mesh(const std::string& objPath, Shader* shader, unsigned int texID = 0, float scale = 1.0f);
    ~Mesh();
    void draw(const glm::mat4& view, const glm::mat4& projection) override;
    glm::vec3 getColor() override { return color; }
    void setColor(glm::vec3 c) override { color = c; }

private:
    unsigned int VAO = 0, VBO = 0;
    unsigned int textureID = 0;
    int vertexCount = 0;
    glm::vec3 color{1.0f};
    float meshScale = 1.0f;
};

#endif
