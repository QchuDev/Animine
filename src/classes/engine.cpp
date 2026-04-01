#include "classes/engine.h"
#include <glad/glad.h>
#include <iostream>

Engine::Engine() : window(nullptr), renderer(nullptr) {}

/**
 * Setup of the engine, creates the window and render
 */
bool Engine::init(int width, int height, const char* title) {
    /*
        If GLFW fails to start... then we cant do a thing
    */
    if (!glfwInit()) return false;
    
    /*
        We create the window with the specifications given to the engine,
        and check if correctly created 
    */
    window = glfwCreateWindow(width, height, title, NULL, NULL);
    if(!window) {
        glfwTerminate();
        return false;    
    }
    glfwMakeContextCurrent(window);
    
    /*
        ???      
    */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return false;

    /*
        Create the renderer we are going to use
    */
    renderer = new Renderer();
    
    return true;
}

/**
 * Main loop runs here
 *  - Draws
 *  - Logic
 *  - Physics
 * etc...
 */
void Engine::run() {
    while(!glfwWindowShouldClose(window)) {
        processInput();
        
        /*
            The engine delegates the drawing to the renderer
        */
        renderer->clear(0.1f, 0.1f, 0.1f, 1.0f);
        renderer->drawScene();
        
        
        /*
            We make sure to ... ???
        */
        glfwSwapBuffers(window);
        glfwPollEvents();
        
    }
}

/**
 * Processes every input made by the user.
 * 
 * example:
 * We set the window to close when the escape key is pressed
 */
void Engine::processInput() {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

/**
 * Destoying correctly the Engine --> with the renderer and terminating the GLFW
 */
Engine::~Engine() {
    delete renderer;
    glfwTerminate();
}
