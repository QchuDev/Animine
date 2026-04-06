#include "classes/scene.h"
#include <fstream>
#include <iostream>

Scene::Scene() {}

bool Scene::init(std::string path) {
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
    
    // for now we print the line...
    std::cout << line << std::endl;
    
    
    
    
}

std::vector<IEntity> Scene::getAllEntities() {
    
}


Scene::~Scene() {}