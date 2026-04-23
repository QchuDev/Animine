#include "classes/render/renderer.h"

#include "classes/entities/axises.h"
#include "classes/render/camera.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>

Renderer::Renderer() {
    // Settings of OpenGL
    // ex: Depth test??
      
    
    // Main Camera
    camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
    
    // The routes should match the vertex and fragment .glsl in our project
    // Basic Shader 
    gizmoShader = new Shader("shaders/line_vertex.glsl", "shaders/line_fragment.glsl");
    textureShader = new Shader("shaders/quad_vertex.glsl", "shaders/quad_fragment.glsl");
    
    // Global axis XYZ
    axises = new Axises(gizmoShader);
    
    // Enables the depth test of the depth ?? lol
    
}

/**
 * Clears the screen --> draws every pixel with the specified RGBA color
 */
void Renderer::clear(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

/**
 * Draws every entity in the scene
 */
void Renderer::drawScene(const std::map<std::string, IEntity*>& entities) {
    
    
    // Global Matrices -> Projection and view
    
    // projection (fov, aspect, near, far)
    glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1280.0f/720.0f, 0.01f, 100.0f);
    glm::mat4 view = camera->GetViewMatrix();
    
    // Drawing the main axis
    axises->draw(view, projection);
    
    // Start the drawing of each entity
    for (auto const& [id, entity] : entities) {
        entity->draw(view, projection);
    }
    
    
}


unsigned int Renderer::getOrCreateTexture(const std::string& fileName) {
    if (loadedTextures.find(fileName) != loadedTextures.end()) {
        return loadedTextures[fileName];
    }

    std::string fullPath = "assets/textures/" + fileName;
    unsigned int id = loadTextureFromDisk(fullPath.c_str());
    loadedTextures[fileName] = id;
    return id;
}
        

unsigned int Renderer::loadTextureFromDisk(const char* path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    stbi_set_flip_vertically_on_load(true); 
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);

    if (data) {
        GLenum format = (nrComponents == 4) ? GL_RGBA : GL_RGB;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Configuración de repetición y filtrado (Mínimo necesario)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        stbi_image_free(data);
        return 0; // O una textura por defecto
    }

    return textureID;
}