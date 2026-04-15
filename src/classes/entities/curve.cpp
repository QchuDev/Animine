#include <glm/glm.hpp>
#include "classes/entities/curve.h"


/**
 * Creation of the curve entity. We specify the formula to use, color and shader.
 */
Curve::Curve(ParamFunction formula, glm::vec3 color, Shader* s) 
: IEntity(s) 
{
    
    // point : [ x,y,z, r,g,b ]
    // line : [point, point]
    std::vector<float> vertices;
    float l = 10.0f;
    float step = 0.1f;
    
    m_vertexCount = 0;
    float t_min = -l;
    float t_max = l;
    
    for (float t = t_min; t <= t_max; t += step) {
        glm::vec3 pos = formula(t);
        
        // Añadir Posición (x, y, z)
        vertices.push_back(pos.x);
        vertices.push_back(pos.y);
        vertices.push_back(pos.z);
        
        // Añadir Color (r, g, b) -> El mismo para todos los puntos de esta curva
        vertices.push_back(color.r);
        vertices.push_back(color.g);
        vertices.push_back(color.b);
        
        m_vertexCount++;
    }
    
    // STUDY THIS FUCKING STUFF
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    // Usamos vertices.size() * sizeof(float) porque es un std::vector dinámico
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    // Atributo de Posición (0): 3 floats, salto de 6 (stride)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Atributo de Color (1): 3 floats, empieza en el byte 12 (3 * float)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void Curve::draw(const glm::mat4& view, const glm::mat4& projection) {
    glBindVertexArray(VAO);
    // IMPORTANTE: Usamos GL_LINE_STRIP para conectar los puntos en cadena
    glDrawArrays(GL_LINE_STRIP, 0, m_vertexCount);
}


Curve::~Curve() {}
