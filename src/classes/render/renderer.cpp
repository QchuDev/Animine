#include "classes/render/renderer.h"

#include "classes/entities/line.h"
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
    strokeShader = new Shader("shaders/stroke_vertex.glsl", "shaders/stroke_fragment.glsl");
    bgShader = new Shader("shaders/bg_vertex.glsl", "shaders/bg_fragment.glsl");

    // Load default stroke texture
    defaultStrokeTexture = getOrCreateTexture("strokes/line_default.png");

    // Create axis lines [-0.2, 0.2]
    axisX = new Line(glm::vec3(-0.2f, 0, 0), glm::vec3(0.2f, 0, 0), glm::vec3(1, 0, 0), strokeShader, defaultStrokeTexture);
    axisY = new Line(glm::vec3(0, -0.2f, 0), glm::vec3(0, 0.2f, 0), glm::vec3(0, 1, 0), strokeShader, defaultStrokeTexture);
    axisZ = new Line(glm::vec3(0, 0, -0.2f), glm::vec3(0, 0, 0.2f), glm::vec3(0, 0, 1), strokeShader, defaultStrokeTexture);

    setupBgQuad();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Renderer::setupBgQuad() {
    // Fullscreen quad in NDC: positions (vec2) + UVs (vec2)
    float quad[] = {
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f,
    };

    glGenVertexArrays(1, &bgVAO);
    glGenBuffers(1, &bgVBO);
    glBindVertexArray(bgVAO);
    glBindBuffer(GL_ARRAY_BUFFER, bgVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Renderer::drawBackground() {
    if (bgTextureID == 0) return;

    glDepthMask(GL_FALSE);

    bgShader->use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bgTextureID);
    bgShader->setInt("bgTexture", 0);

    glBindVertexArray(bgVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    glDepthMask(GL_TRUE);
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
    float aspect = 1280.0f / 720.0f;
    glm::mat4 projection;
    if (useOrtho) {
        float h = orthoZoom;
        float w = h * aspect;
        projection = glm::ortho(-w, w, -h, h, 0.01f, 100.0f);
    } else {
        projection = glm::perspective(glm::radians(90.0f), aspect, 0.01f, 100.0f);
    }
    glm::mat4 view = camera->GetViewMatrix();

    axisX->draw(view, projection);
    axisY->draw(view, projection);
    axisZ->draw(view, projection);

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