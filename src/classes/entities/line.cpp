#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "classes/entities/line.h"

Line::Line(glm::vec3 startPos, glm::vec3 endPos, glm::vec3 color, Shader* s, unsigned int strokeTex)
    : IEntity(s), startPos(startPos), endPos(endPos), color(color), textureID(strokeTex)
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // 4 verts * 5 floats (pos3 + uv2)
    glBufferData(GL_ARRAY_BUFFER, 4 * 5 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // UV
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Line::buildQuad(const glm::vec3& camPos) {
    glm::vec3 dir = endPos - startPos;
    glm::vec3 toCamera = camPos - (startPos + endPos) * 0.5f;

    // Perpendicular to both line direction and view direction
    glm::vec3 side = glm::normalize(glm::cross(dir, toCamera)) * width;

    // If cross is degenerate (line points at camera), fallback to up
    if (glm::length(side) < 0.0001f)
        side = glm::normalize(glm::cross(dir, glm::vec3(0, 1, 0))) * width;

    glm::vec3 v0 = startPos - side;
    glm::vec3 v1 = startPos + side;
    glm::vec3 v2 = endPos + side;
    glm::vec3 v3 = endPos - side;

    // Simple UVs: full texture stretch (3-slice handled later in shader if needed)
    float vertices[] = {
        v0.x, v0.y, v0.z,  0.0f, 0.0f,
        v1.x, v1.y, v1.z,  0.0f, 1.0f,
        v2.x, v2.y, v2.z,  1.0f, 1.0f,
        v3.x, v3.y, v3.z,  1.0f, 0.0f,
    };

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
}

void Line::draw(const glm::mat4& view, const glm::mat4& projection) {
    // Extract camera position from inverse view matrix
    glm::mat4 invView = glm::inverse(view);
    glm::vec3 camPos = glm::vec3(invView[3]);

    buildQuad(camPos);

    shader->use();
    shader->setMat4(shader->modelLoc, transform.getModelMatrix());
    shader->setMat4(shader->viewLoc, view);
    shader->setMat4(shader->projLoc, projection);

    // Set tint color
    int tintLoc = glGetUniformLocation(shader->ID, "tintColor");
    glUniform3f(tintLoc, color.r, color.g, color.b);

    // Bind stroke texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    shader->setInt("strokeTexture", 0);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
}

Line::~Line() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}
