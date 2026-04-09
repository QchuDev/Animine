#ifndef RENDERER_H
#define RENDERER_H

#include "classes/axises.h"
#include "classes/shader.h"
#include "classes/camera.h"

#include "classes/entities/entity.h"

#include <string>
#include <map>

class Renderer {
    private: 
    Shader* textureShader;
    Shader* gizmoShader;
    
    public:
        Axises* axises;
        Camera* camera;
        
        Renderer();
        void clear(float r, float g, float b, float a); // Cleans the screen with a specified color
        void drawScene(std::map<std::string, IEntity*>&); // Draws on screen the current scene 

        // Getters
        Shader* getGizmoShader() const { return gizmoShader; }
        Shader* getTextureShader() const { return textureShader; }
    

};


#endif