#ifndef SCENESPARSER.H
#define SCENESPARSER.H

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <sstream>
#include <filesystem>

// Forward declarations para evitar acoplamiento innecesario en el .h
class Renderer;
class Animator;
class Scene;

namespace fs = std::filesystem;

class ScenesParser {
private:
    // References of the animator and renderer
    Renderer* renderer;
    Animator* animator;
    
    /**
     * @brief Procesa una línea individual del archivo.
     * Es llamado por parseFile por cada línea válida.
     */
    void parseLine(const std::string& line, Scene* scene);

public:
    ScenesParser(Renderer* r, Animator* a) : renderer(r), animator(a) {}

    /**
     * @brief Itera sobre la carpeta /scenes y retorna todas las escenas encontradas.
     * @param folder_path Ruta a la carpeta que contiene los .txt
     */
    std::vector<std::unique_ptr<Scene>> extractScenes(const std::string& folder_path);

    /**
     * @brief Lee un archivo específico y construye una instancia de Scene.
     * @param file_path Ruta completa al archivo .txt
     */
    std::unique_ptr<Scene> parseFile(const std::string& file_path);
};
#endif