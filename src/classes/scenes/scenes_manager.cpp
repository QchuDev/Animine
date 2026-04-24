#include "classes/scenes/scenes_manager.h"

#include <filesystem>
#include <iostream>
#include <string>

// Alias para acortar el código
namespace fs = std::filesystem;

bool ScenesManager::loadScenes(std::string folder_path) {
    try {
        // Verificamos si la ruta existe y es un directorio
        if (!fs::exists(folder_path) || !fs::is_directory(folder_path)) {
            std::cerr << "Error: La ruta no existe o no es un directorio: " << folder_path << std::endl;
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
                // Scene* newScene = interpreter->parse(filePath);
                // this->scenes[fileName] = newScene;
            }
        }
        return true;

    } catch (const fs::filesystem_error& e) {
        std::cerr << "Excepción de filesystem: " << e.what() << std::endl;
        return false;
    }
}