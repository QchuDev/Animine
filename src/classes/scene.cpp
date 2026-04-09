#include <iostream>
#include <fstream>          // For reading the .txt file
#include <sstream>          // For the parsing of text

#include "classes/scene.h"
#include "classes/entities/line.h"
#include "classes/renderer.h"

enum class EntityType {
    LINE,
    CURVE,
    QUAD,
    UNKNOWN
};

EntityType getEntityType(const std::string& type) {
    if (type == "line") return EntityType::LINE;
    if (type == "curve") return EntityType::CURVE;
    if (type == "line") return EntityType::QUAD;
    return EntityType::UNKNOWN;
}


bool Scene::loadScene(std::string path) {
    // Creates an input file stream and open the file 
    std::ifstream myFile(path);
    
    // Check if the file was opened successfullu
    if (!myFile.is_open()) {
        std::cerr << "Error: Could not find the specified file" << std::endl;
        return false;
    }
    
    // we read and pring (for now...) the content of the file
    std::string line;
    std::cerr << "Reading file ..." << std::endl;
    while (std::getline(myFile, line))
    {
        parseLine(line);        
    }
    
    // Close the file
    myFile.close();
    return true;
}


void Scene::parseLine(const std::string& line) {
    // We ignore comments -> lines starting with '#'
    if (line.empty() || line[0] == '#') { return; }
    std::cout << "Processing --> " + line << std::endl; // temp.
    
    std::stringstream ss(line);
    std::string typeStr;
    ss >> typeStr;     // Extracts the firt word --> the object type (example: line)
    
    EntityType type = getEntityType(typeStr); 
    
    switch (type) {
        case EntityType::LINE: {
            std::string id;
            float x1, y1, z1, x2, y2, z2, r, g, b;
            
            if (ss >> id >> x1 >> y1 >> z1 >> x2 >> y2 >> z2 >> r >> g >> b) {
                if (entities.find(id) == entities.end()) {
                    entities[id] = new Line(glm::vec3(x1,y1,z1), glm::vec3(x2,y2,z2), glm::vec3(r,g,b), renderer->getGizmoShader());
                } else {
                    std::cout << "Error: duplicated id at .txt" << std::endl;
                }
            }
            
            break;
        }
        default: {
            std::cerr << "Unknown entity type: " << typeStr << std::endl;
            break;
        }
    }
    
}


std::map<std::string, IEntity*>& Scene::getAllEntities() {
    return entities;
}


Scene::~Scene() {
    for (auto const& [id, entity] : entities) {
        delete entity;
    }
    entities.clear();
}