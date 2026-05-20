#include <glad/glad.h>
#include "classes/entities/quad.h"

// Constructor, we should pass the four vertices
Quad::Quad(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4, unsigned int tex, Shader* s) 
        : IEntity(s), textureID(tex) {
    // Add the points to the points array
    points[0]=p1; points[1]=p2; points[2]=p3; points[3]=p4;

    // Coordinates in uvs go from 0-1 in each x and y
    uvs[0] = glm::vec2(0.0f, 0.0f); // Bottom left
    uvs[1] = glm::vec2(1.0f, 0.0f); // Bottom right
    uvs[2] = glm::vec2(1.0f, 1.0f); // Superior right   
    uvs[3] = glm::vec2(0.0f, 1.0f); // Superior left
    
    setupMesh();
}

void Quad::setupMesh() {
    // Creamos un array que combina Posición (3) + UV (2) = 5 floats por vértice
    float vertices[] = {
        points[0].x, points[0].y, points[0].z,  uvs[0].x, uvs[0].y,
        points[1].x, points[1].y, points[1].z,  uvs[1].x, uvs[1].y,
        points[2].x, points[2].y, points[2].z,  uvs[2].x, uvs[2].y,
        points[3].x, points[3].y, points[3].z,  uvs[3].x, uvs[3].y
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Atributo 0: Posición (x, y, z)
    // El "stride" es 5 * sizeof(float) porque cada vértice ocupa 5 floats
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Atributo 1: UVs (u, v)
    // El "offset" es 3 * sizeof(float) porque los UVs empiezan después del vec3 de posición
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Quad::draw(const glm::mat4& view, const glm::mat4& projection) {
    shader->use();
    
    shader->setMat4(shader->modelLoc, transform.getModelMatrix());
    shader->setMat4(shader->viewLoc, view);
    shader->setMat4(shader->projLoc, projection);

    int tintLoc = glGetUniformLocation(shader->ID, "tintColor");
    glUniform3f(tintLoc, color.r, color.g, color.b);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    shader->setInt("ourTexture", 0);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
}

Quad::~Quad() {}
