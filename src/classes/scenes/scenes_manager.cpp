#include "classes/scenes/scenes_manager.h"
#include <filesystem>
#include <iostream>
#include <string>

ScenesManager::ScenesManager(Renderer* r, Animator* a): renderer(r), animator(a) {
    interpreter = new Interpreter("./assets/scenes/");
}

// Alias para acortar el código
namespace fs = std::filesystem;
bool ScenesManager::loadScenes(std::string folder_path) {
    try {
        // Verificamos si la ruta existe y es un directorio
        if (!fs::exists(folder_path) || !fs::is_directory(folder_path)) {
            std::cerr << "Error: Path doesnt exist or is not a folder: " << folder_path << std::endl;
            return false;
        }

        // Iterador de directorio (Range-based for loop)
        for (const auto& entry : fs::directory_iterator(folder_path)) {
            
            // Verificamos que sea un archivo regular y tenga extensión .txt
            if (entry.is_regular_file() && entry.path().extension() == ".txt") {
                
                std::string filePath = entry.path().string();
                std::string fileName = entry.path().stem().string(); // Nombre sin extensión

                std::cout << "Cargando escena: " << fileName << " desde " << filePath << std::endl;

                // Aquí llamarías a tu Interpreter/Parser
                Scene* newScene = new Scene(
                    interpreter->getEntities(filePath), 
                    interpreter->getAnimations(filePath),
                    renderer, animator
                );
                
                this->scenes[fileName] = newScene;
            }
        }
        return true;

    } catch (const fs::filesystem_error& e) {
        std::cerr << "Excepción de filesystem: " << e.what() << std::endl;
        return false;
    }
}

Scene* ScenesManager::getCurrentScene() {
    return current_scene;
}