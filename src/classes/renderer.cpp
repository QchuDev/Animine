#include "classes/axises.h"
#include "classes/renderer.h"
#include "classes/camera.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Renderer::Renderer() {
    // Settings of OpenGL
    // ex: Depth test??
      
    // Global axis XYZ
    axises = new Axises();
    
    // Main Camera
    camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));

    // The routes should match the vertex and fragment .glsl in our project
    // Basic Shader 
    basicShader = new Shader("shaders/vertex.glsl", "shaders/fragment.glsl");
    

}

/**
 * Clears the screen --> draws every pixel with the specified RGBA color
 */
void Renderer::clear(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

/**
 * Draws every entity in the scene
 */
void Renderer::drawScene() {
    basicShader->use(); // activate the shader before drawing
    
    // Projection Matrix
    // 45 degrees of view, 16:9 aspect ratio, close-up and far planes
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1280.0f/720.0f, 0.1f, 100.0f);
    
    // View Matrix
    glm::mat4 view = camera->GetViewMatrix();
    
    // Model matrix
    glm::mat4 model = glm::mat4(1.0f);
    
    // Send matrices to shader
    // model, view, projection should be the same names as the shaders/vertex.glsl
    glUniformMatrix4fv(glGetUniformLocation(basicShader->ID, "model"), 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(basicShader->ID, "view"), 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(basicShader->ID, "projection"), 1, GL_FALSE, &projection[0][0]);
    
    axises->draw(*basicShader, view, projection);
}