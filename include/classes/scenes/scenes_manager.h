#ifndef SCENES_MANAGER_H
#define SCENES_MANAGER_H

#include <iostream>
#include "scene.h"
#include "animator.h"
class ScenesManager {
    public:
        Scene& getCurrentScene();
        Scene& getScene(std::string& scene_name);

        bool init();

    private:

        // The scene to be used
        Scene current_scene;
        
        // All uploaded scenes
        std::map<std::string, Scene> scenes;
        
        // The component responsible of the motion 
        // of each Entity on the current scene
        Animator animator;
        
};


#endif