#include "classes/axises.h"

Axises::Axises() {
    // point : [x,y,z,r,g,b]
    // line : [point, point]
    // 3 axis/lines -> 6 points
    float vertices[] = {
        //     XYZ               RGB
        -5.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // X-axis start
        5.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // X-axis end
        0.0f, -5.0f, 0.0f, 0.0f, 1.0f, 0.0f, // Y-axis start
        0.0f, 5.0f, 0.0f, 0.0f, 1.0f, 0.0f, // Y-axis end
        0.0f, 0.0f, -5.0f, 0.0f, 0.0f, 1.0f, // Z-axis start
        0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 1.0f, // Z-axis end
    };
    
    // STUDY THIS FUCKING STUFF
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    
    // Positions attr.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Color attr.
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    
}

void Axises::draw(Shader& shader, const glm::mat4& view, const glm::mat4& projection) {
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, 6);
};
