#include <glad/glad.h>
#include <glm/glm.hpp>
#include <iostream>
#include <numbers>

#include "classes/engine.h"
#include "classes/animations/animator.h"
#include "classes/scenes/scenes_manager.h"
#include "classes/creation/scenes_parser.h"
#include "classes/paths.h"


Engine::Engine() : window(nullptr), renderer(nullptr), animator(nullptr), scenesManager(nullptr) {}

/**
 * Setup of the engine, creates the window and render
 */
bool Engine::init(int width, int height, const char* title) {

    if (!glfwInit()) return false;                                  // Check if glfw fails to init
    
    window = glfwCreateWindow(width, height, title, NULL, NULL);    // We create the window with the specifications given to the engine
    if(!window) {                                                   // check if correctly created
        glfwTerminate();
        return false;    
    }
    glfwMakeContextCurrent(window);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return false;
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    renderer = new Renderer(); // Create the renderer to draw entities on the window
    animator = new Animator();
    
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // V-sync activation
    glfwSwapInterval(1);
    
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
    // Build all scenes from assets/scenes/ and hand them to the manager
    ScenesParser parser(renderer);
    auto scenes = parser.extractScenes(assetPath("assets/scenes/"));
    scenesManager = new ScenesManager(std::move(scenes));

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput();

        Scene* scene = scenesManager->getCurrentScene();
        if (!scene) { glfwSwapBuffers(window); glfwPollEvents(); continue; }

        renderer->clear(0.1f, 0.1f, 0.1f, 1.0f);
        animator->update(deltaTime, scene);
        renderer->drawScene(scene->getEntities());

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
    
    // The cursor input logic
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
    
    // The camera takes this inputs to rotate 
    renderer->camera->ProcessMouseMovement(xoffset, yoffset); 
}

/**
 * Destoying correctly the Engine --> with the renderer and terminating the GLFW
 */
Engine::~Engine() {
    delete scenesManager;
    delete animator;
    delete renderer;
    glfwTerminate();
}
