#ifndef ENGINE_H
#define ENGINE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <filesystem>
#include <map>
#include <string>
#include "classes/render/renderer.h"
#include "classes/animations/animator.h"
#include "classes/scenes/scenes_manager.h"

class ScenesParser;

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
    ScenesParser* parser = nullptr;

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    bool rightPressed = false;
    bool leftPressed = false;
    bool tabPressed = false;
    bool gPressed = false;
    // guiMode = true  -> cursor visible, camara solo con click derecho (para GUI)
    // guiMode = false -> modo normal/FPS: cursor capturado, camara siempre activa
    bool guiMode = false;
    int frameCount = 0;

    std::map<std::string, std::filesystem::file_time_type> fileTimestamps;

    void processInput();
    void checkHotReload();
};

#endif
