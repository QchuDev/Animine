#ifndef RENDERER_H
#define RENDERER_H

#include "classes/entities/entity.h"

#include "classes/entities/axises.h"
#include "classes/render/shader.h"
#include "classes/render/camera.h"

#include <string>
#include <map>

class Renderer {
    private: 
        Shader* textureShader;
        Shader* gizmoShader;
        std::map<std::string, unsigned int> loadedTextures;
    public:
        Axises* axises;
        Camera* camera;
        
        Renderer();
        void clear(float r, float g, float b, float a); // Cleans the screen with a specified color
        void drawScene(const std::map<std::string, IEntity*>& entities); // Draws on screen the current scene 

        // Getters
        Shader* getGizmoShader() const { return gizmoShader; }
        Shader* getTextureShader() const { return textureShader; }
        
        // The renderer handles the textures, we use the same textures for various entities
        unsigned int getOrCreateTexture(const std::string& fileName);
        
        // Function with glGenTextures, glTexImage2D, etc.
        unsigned int loadTextureFromDisk(const char* path);

};


#endif