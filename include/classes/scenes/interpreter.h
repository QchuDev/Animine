#ifndef INTERPRETER_H
#define INTERPRETER_H
#include <iostream>
#include "classes/entities/entity.h"
#include "classes/animations/animation.h"

/**
 * The Interpreter gets 
 */
class Interpreter {
    private:
        std::string file_path; 
        IEntity entityCreation(std::string& type, std::stringstream& ss);
        void parseLine(const std::string& line);
        
    public:
        Interpreter();
        std::map<std::string, IEntity*> getEntities(std::string& file_name);
        std::vector<std::vector<IAnimation*>> getAnimations(std::string& file_name);
            
};


#endif