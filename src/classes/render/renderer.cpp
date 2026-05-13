#include "classes/render/renderer.h"

#include "classes/entities/axises.h"
#include "classes/render/camera.h"
#include "classes/paths.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>
#include <algorithm>
#include <vector>

Renderer::Renderer() {
    camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));

    gizmoShader = new Shader("shaders/line_vertex.glsl", "shaders/line_fragment.glsl");
    textureShader = new Shader("shaders/quad_vertex.glsl", "shaders/quad_fragment.glsl");

    axises = new Axises(gizmoShader);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

/**
 * Clears the screen --> draws every pixel with the specified RGBA color
 */
void Renderer::clear(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

/**
 * Draws every entity in the scene, sorted back-to-front for correct alpha blending
 */
void Renderer::drawScene(const std::map<std::string, IEntity*>& entities) {
    glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1280.0f/720.0f, 0.01f, 100.0f);
    glm::mat4 view = camera->GetViewMatrix();

    axises->draw(view, projection);

    // Sort entities back-to-front (farthest first)
    std::vector<IEntity*> sorted;
    sorted.reserve(entities.size());
    for (auto const& [id, entity] : entities)
        sorted.push_back(entity);

    glm::vec3 camPos = camera->Position;
    std::sort(sorted.begin(), sorted.end(), [&camPos](IEntity* a, IEntity* b) {
        float da = glm::length(a->transform.position - camPos);
        float db = glm::length(b->transform.position - camPos);
        return da > db; // farthest first
    });

    for (IEntity* entity : sorted)
        entity->draw(view, projection);
}


unsigned int Renderer::getOrCreateTexture(const std::string& fileName) {
    if (loadedTextures.find(fileName) != loadedTextures.end()) {
        return loadedTextures[fileName];
    }

    std::string fullPath = assetPath("assets/textures/" + fileName);
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