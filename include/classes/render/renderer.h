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
        Shader* strokeShader;
        Shader* bgShader;
        unsigned int bgVAO = 0, bgVBO = 0;
        unsigned int bgTextureID = 0;
        std::map<std::string, unsigned int> loadedTextures;
        unsigned int defaultStrokeTexture = 0;

        void setupBgQuad();
    public:
        Axises* axises;
        Camera* camera;
        
        Renderer();
        void clear(float r, float g, float b, float a);
        void drawBackground();
        void drawScene(const std::map<std::string, IEntity*>& entities);

        // Getters
        Shader* getGizmoShader() const { return gizmoShader; }
        Shader* getTextureShader() const { return textureShader; }
        Shader* getStrokeShader() const { return strokeShader; }
        unsigned int getDefaultStrokeTexture() const { return defaultStrokeTexture; }
        
        void setBackground(unsigned int texID) { bgTextureID = texID; }

        // The renderer handles the textures, we use the same textures for various entities
        unsigned int getOrCreateTexture(const std::string& fileName);
        
        // Function with glGenTextures, glTexImage2D, etc.
        unsigned int loadTextureFromDisk(const char* path);

};


#endif