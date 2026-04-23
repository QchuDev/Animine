#ifndef SCENES_MANAGER_H
#define SCENES_MANAGER_H

#include <iostream>
#include "classes/scenes/scene.h"
#include "classes/animations/animator.h"
class ScenesManager {
private:
    // The scene to be used
    Scene current_scene;
    
    // All uploaded scenes
    std::map<std::string, Scene> scenes;
    
    // The component responsible of the motion 
    // of each Entity on the current scene
    Animator animator;

public:
        Scene& getCurrentScene();
        Scene& getScene(std::string& scene_name);
        
        bool init();
    
};


#endif