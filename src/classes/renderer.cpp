#include "classes/axises.h"
#include "classes/renderer.h"
#include "classes/camera.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Renderer::Renderer() {
    // Settings of OpenGL
    // ex: Depth test??
      
    
    // Main Camera
    camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
    
    // The routes should match the vertex and fragment .glsl in our project
    // Basic Shader 
    gizmoShader = new Shader("shaders/vertex.glsl", "shaders/line_fragment.glsl");
    textureShader = new Shader("shaders/vertex.glsl", "shaders/quad_fragment.glsl");
    
    // Global axis XYZ
    axises = new Axises(gizmoShader);

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
void Renderer::drawScene(std::map<std::string, IEntity*>& entities) {
    
    
    // Global Matrices -> Projection and view
    
    // projection (fov, aspect, near, far)
    glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1280.0f/720.0f, 0.01f, 100.0f);
    glm::mat4 view = camera->GetViewMatrix();
    
    
    // Start the drawing of each entity
    for (auto const& [id, entity] : entities) {
        entity->draw(view, projection);
    }
    
    // Drawing the main axis
    gizmoShader->use(); // activate the gizmo shader before drawing
    axises->draw(view, projection);
    
}