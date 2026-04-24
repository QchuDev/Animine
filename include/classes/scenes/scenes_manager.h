#ifndef SCENES_MANAGER_H
#define SCENES_MANAGER_H

#include <iostream>
#include "classes/scenes/scene.h"
#include "classes/animations/animator.h"
#include "classes/scenes/interpreter.h"

class ScenesManager {
private:
    
    Scene* current_scene;                    // The scene to be used
    std::map<std::string, Scene*> scenes;    // All uploaded scenes
    Interpreter* interpreter;
    Renderer* renderer;
    Animator* animator;
    
public:
    ScenesManager(Renderer* renderer, Animator* Animator);
    ~ScenesManager();
    Scene* getCurrentScene();
    Scene* getScene(std::string& scene_name);
    bool loadScenes(std::string folder_path);
};


#endif