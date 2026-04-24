#include <iostream>
#include <fstream>          // For reading the .txt file
#include <sstream>          // For the parsing of text

// Managers
#include "classes/scenes/scene.h"
#include "classes/render/renderer.h"

Scene::Scene(std::map<std::string, IEntity> entities, 
        std::map<std::string, IAnimation> animations, 
        Renderer* r, Animator* a) : renderer(r), animator(a) {
    
    
    
    
}

const std::map<std::string, IEntity*>& Scene::getAllEntities() const {
    return entities;
}



Scene::~Scene() {
    for (auto const& [id, entity] : entities) {
        delete entity;
    }
    entities.clear();
}