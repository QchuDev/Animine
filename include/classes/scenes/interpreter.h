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
    public:
        Interpreter(std::string filePath);
        std::vector<IEntity> getEntities(std::string& file_name);
        std::vector<IAnimation> getAnimations(std::string& file_name);
            
};


#endif