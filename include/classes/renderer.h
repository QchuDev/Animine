#ifndef RENDERER_H
#define RENDERER_H

#include "classes/axises.h"
#include "classes/shader.h"
#include "classes/camera.h"

#include "classes/entities/entity.h"

class Renderer {
    public:
        Renderer();
        Axises* axises;
        Shader* basicShader;
        Camera* camera;
        
        void clear(float r, float g, float b, float a); // Cleans the screen with a specified color
        void drawScene(std::vector<IEntity*>&); // Draws on screen the current scene 
};


#endif