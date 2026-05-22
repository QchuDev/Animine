#include <glad/glad.h>
#include <glm/glm.hpp>
#include <iostream>
#include <numbers>
#include <filesystem>

#include "classes/engine.h"
#include "classes/animations/animator.h"
#include "classes/scenes/scenes_manager.h"
#include "classes/creation/scenes_parser.h"
#include "classes/paths.h"

namespace fs = std::filesystem;


Engine::Engine() : window(nullptr), renderer(nullptr), animator(nullptr), scenesManager(nullptr) {}

/**
 * Setup of the engine, creates the window and render
 */
bool Engine::init(int width, int height, const char* title) {

    if (!glfwInit()) return false;

    glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);                       // Always on top

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
    
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    
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
    parser = new ScenesParser(renderer);
    std::string scenesPath = assetPath("assets/scenes/");
    auto scenes = parser->extractScenes(scenesPath);

    // Record initial timestamps
    for (auto& entry : fs::directory_iterator(scenesPath)) {
        if (entry.path().extension() == ".txt")
            fileTimestamps[entry.path().string()] = fs::last_write_time(entry);
    }

    scenesManager = new ScenesManager(std::move(scenes));

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput();

        // Hot reload check every 60 frames
        if (++frameCount >= 60) {
            frameCount = 0;
            checkHotReload();
        }

        Scene* scene = scenesManager->getCurrentScene();
        if (!scene) { glfwSwapBuffers(window); glfwPollEvents(); continue; }

        renderer->clear(0.1f, 0.1f, 0.1f, 1.0f);

        // Set background from scene
        if (!scene->backgroundTexture.empty())
            renderer->setBackground(renderer->getOrCreateTexture(scene->backgroundTexture));
        else
            renderer->setBackground(0);

        renderer->drawBackground();
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

    // Scene cycling with arrow keys (with debounce)
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS && !rightPressed) {
        scenesManager->nextScene();
        animator->reset(scenesManager->getCurrentScene());
        rightPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_RELEASE) rightPressed = false;

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS && !leftPressed) {
        scenesManager->prevScene();
        animator->reset(scenesManager->getCurrentScene());
        leftPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_RELEASE) leftPressed = false;

    // Toggle ortho/perspective with Tab
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS && !tabPressed) {
        renderer->useOrtho = !renderer->useOrtho;
        tabPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE) tabPressed = false;

    // Ortho zoom with Q/E
    if (renderer->useOrtho) {
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
            renderer->orthoZoom += 5.0f * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            renderer->orthoZoom -= 5.0f * deltaTime;
        if (renderer->orthoZoom < 0.5f) renderer->orthoZoom = 0.5f;
    }

    // Camera movement only with right-click held
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) != GLFW_PRESS) {
        firstMouse = true;
        return;
    }

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
 * Checks if any scene file was modified, added, or deleted and updates accordingly
 */
void Engine::checkHotReload() {
    std::string scenesPath = assetPath("assets/scenes/");

    // Detect deleted files
    std::vector<std::string> toRemove;
    for (auto& [path, lastTime] : fileTimestamps) {
        if (!fs::exists(path)) {
            std::string id = fs::path(path).stem().string();
            scenesManager->removeScene(id);
            toRemove.push_back(path);
        }
    }
    for (auto& p : toRemove) fileTimestamps.erase(p);

    // Detect new and modified files
    try {
        for (auto& entry : fs::directory_iterator(scenesPath)) {
            if (entry.path().extension() != ".txt") continue;
            std::string path = entry.path().string();
            auto currentTime = fs::last_write_time(entry);

            if (fileTimestamps.find(path) == fileTimestamps.end()) {
                // New file
                fileTimestamps[path] = currentTime;
                auto scene = parser->parseFile(path);
                if (scene) scenesManager->addScene(std::move(scene));
            } else if (currentTime != fileTimestamps[path]) {
                // Modified file
                fileTimestamps[path] = currentTime;
                auto scene = parser->parseFile(path);
                if (!scene) continue;
                std::string id = scene->id;
                bool isActive = (scenesManager->getCurrentScene() &&
                                 scenesManager->getCurrentScene()->id == id);
                scenesManager->replaceScene(id, std::move(scene));
                if (isActive)
                    animator->reset(scenesManager->getCurrentScene());
            }
        }
    } catch (...) {}
}

/**
 * Destoying correctly the Engine --> with the renderer and terminating the GLFW
 */
Engine::~Engine() {
    delete scenesManager;
    delete animator;
    delete parser;
    delete renderer;
    glfwTerminate();
}
