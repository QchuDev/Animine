#include "classes/axises.h"
#include "classes/renderer.h"
#include <glad/glad.h>

Renderer::Renderer() {
    // Settings of OpenGL
    // ex: Depth test??  
    axises = new Axises();
    
    // The routes should match the vertex and fragment .glsl in our project
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
    axises->draw();

}


