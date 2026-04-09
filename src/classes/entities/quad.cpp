#include "classes/entities/quad.h"

// Constructor, we should pass the four vertices
Quad::Quad(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4, unsigned int tex, Shader* s) 
        : IEntity(s), textureID(tex) 
{
    // Add the points to the points array
    points[0]=p1; points[1]=p2; points[2]=p3; points[3]=p4;

    // Coordinates in uvs go from 0-1 in each x and y
    uvs[0] = glm::vec2(0.0f, 0.0f); // Bottom left
    uvs[1] = glm::vec2(1.0f, 0.0f); // Bottom right
    uvs[2] = glm::vec2(1.0f, 1.0f); // Superior right   
    uvs[3] = glm::vec2(0.0f, 1.0f); // Superior left
    
    // Setup

}

void Quad::draw(const glm::mat4& view, const glm::mat4& projection) {
    shader->use();
    
    // 1. Pasar matrices (asumiendo que tus variables en el shader se llaman así)
    shader->setMat4(shader->modelLoc, transform.getModelMatrix());
    shader->setMat4(shader->viewLoc, view);
    shader->setMat4(shader->projLoc, projection);
    
    // 2. Configurar textura
    glActiveTexture(GL_TEXTURE0); // Activar unidad de textura 0
    glBindTexture(GL_TEXTURE_2D, textureID);
    shader->setInt("ourTexture", 0); // Decirle al shader que use la unidad 0

    // 3. Dibujar
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
    
}

Quad::~Quad() {
    
    
}
