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
    
    // SETS
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
        // Calcular deltaTime
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;   
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

    // For the closing of the window
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    // For the camera movement
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        renderer->camera->ProcessKeyboard(FORWARD, this->deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        renderer->camera->ProcessKeyboard(BACKWARD, this->deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        renderer->camera->ProcessKeyboard(LEFT, this->deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        renderer->camera->ProcessKeyboard(RIGHT, this->deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        renderer->camera->ProcessKeyboard(UP, this->deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        renderer->camera->ProcessKeyboard(DOWN, this->deltaTime);
    }

    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    if (firstMouse) {
        lastX = xpos; lastY = ypos;
        firstMouse = false;
    }
    float xoffset = (float)(xpos - lastX);
    float yoffset = (float)(lastY - ypos); 
    lastX = xpos;
    lastY = ypos;
    renderer->camera->ProcessMouseMovement(xoffset, yoffset);
}

/**
 * Destoying correctly the Engine --> with the renderer and terminating the GLFW
 */
Engine::~Engine() {
    delete renderer;
    glfwTerminate();
}
