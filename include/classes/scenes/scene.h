#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <string>
#include <map>
#include "classes/entities/entity.h"
#include "classes/animations/animation.h"
#include "classes/animations/animator.h"
#include "classes/render/renderer.h"

class Scene {
private:
    std::map<std::string, IEntity*> entities;       // All entities of the scene
    std::map<std::string, IAnimation*> animations;  // All animations of the scene
    
    Renderer* renderer;
    Animator* animator;
    
public:
    Scene(
        std::map<std::string, IEntity> entities, 
        std::vector<IAnimation> animations, 
        Renderer* r, Animator* a);
    
    ~Scene();
    
    // Get
    const std::map<std::string, IEntity*>& getAllEntities() const;
    const std::vector<IAnimation*>& getAllAnimations() const;

    
};

#endif