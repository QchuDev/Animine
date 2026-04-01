#ifndef AXISES_H
#define AXISES_H

#include <glad/glad.h>

class Axises {
    public:
        Axises();
        void draw();
    private:
        unsigned int VAO, VBO;
        
};

#endif