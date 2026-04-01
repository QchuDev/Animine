#ifndef RENDERER_H
#define RENDERER_H

#include "classes/axises.h"
#include "classes/shader.h"


class Renderer {
    public:
        Renderer();
        Axises* axises;
        Shader* basicShader;
        void clear(float r, float g, float b, float a); // Cleans the screen with a specified color
        void drawScene(); // Draws on screen the current scene 
};


#endif