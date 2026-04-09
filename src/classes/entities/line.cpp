#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "classes/entities/line.h"


Line::Line(glm::vec3 startPos, glm::vec3 endPos, glm::vec3 color, Shader* s) 
: IEntity(s) 
{
    // point : [ x,y,z, r,g,b ]
    // line : [point, point]
    // 3 axis/lines -> 6 points
    float vertices[] = {
        //     XYZ               RGB
        startPos.x, startPos.y, startPos.z, color.x, color.y, color.z, // X-axis start
        endPos.x, endPos.y, endPos.z, color.x, color.y, color.z // X-axis end
    };
    
    // STUDY THIS FUCKING STUFF
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    // Passing all vertices and colors 
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    
    // Positions attr.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Color attr.
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
}

void Line::draw(const glm::mat4& view, const glm::mat4& projection) {
    shader->use(); // Asegurarnos de que el shader está activo

    shader->setMat4(shader->modelLoc, transform.getModelMatrix());
    shader->setMat4(shader->viewLoc, view);
    shader->setMat4(shader->projLoc, projection);
    
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, 2);
    glBindVertexArray(0);
}


Line::~Line() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}