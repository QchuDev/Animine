#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <string>
#include <map>
#include "classes/entities/entity.h"
#include "classes/renderer.h"

class Scene {
private:
    std::map<std::string, IEntity*> entities;
    Renderer* renderer;
    void parseLine(const std::string& line);
public:
    Scene(Renderer* r) : renderer(r) {};
    ~Scene();

    /**
     * This init method takes a path as an argument. 
     * It should be a .txt where the wanted entities are specified
     * 
     * returns if it was correctly initialized
     */
    bool loadScene(std::string path);
    
    
    /**
     * Returns a vector with all the entities in this scene
     */
    std::map<std::string, IEntity*>& getAllEntities();

};

#endif