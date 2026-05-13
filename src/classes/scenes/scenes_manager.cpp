#include "classes/scenes/scenes_manager.h"
#include <iostream>

ScenesManager::ScenesManager(std::vector<std::unique_ptr<Scene>> incoming) {
    for (auto& scene : incoming) {
        std::string id = scene->id;
        scenes[id] = std::move(scene);
    }
    if (!scenes.empty())
        currentScene = scenes.begin()->second.get();
}

Scene* ScenesManager::getCurrentScene() const {
    return currentScene;
}

void ScenesManager::setCurrentScene(const std::string& id) {
    auto it = scenes.find(id);
    if (it != scenes.end())
        currentScene = it->second.get();
    else
        std::cerr << "ScenesManager: scene '" << id << "' not found\n";
}
