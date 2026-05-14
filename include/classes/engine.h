#ifndef ENGINE_H
#define ENGINE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "classes/render/renderer.h"
#include "classes/animations/animator.h"
#include "classes/scenes/scenes_manager.h"

class Engine {
public:
    Engine();
    ~Engine();
    bool init(int width, int height, const char* title);
    void run();

    double lastX = 640.0, lastY = 360.0;
    bool firstMouse = true;

private:
    GLFWwindow* window;
    Renderer* renderer;
    Animator* animator;
    ScenesManager* scenesManager = nullptr;

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    bool rightPressed = false;
    bool leftPressed = false;
    void processInput();
};

#endif
