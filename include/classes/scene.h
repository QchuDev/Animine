#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <string>
#include "classes/entities/entity.h"

class Scene {
public:
    Scene();
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
    std::vector<IEntity*>& getAllEntities();

private:
    std::vector<IEntity*> entities;
    void parseLine(const std::string& line);
};

#endif