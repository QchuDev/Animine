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

void ScenesManager::nextScene() {
    if (scenes.empty()) return;
    for (auto it = scenes.begin(); it != scenes.end(); ++it) {
        if (it->second.get() == currentScene) {
            ++it;
            currentScene = (it != scenes.end()) ? it->second.get() : scenes.begin()->second.get();
            return;
        }
    }
}

void ScenesManager::prevScene() {
    if (scenes.empty()) return;
    for (auto it = scenes.begin(); it != scenes.end(); ++it) {
        if (it->second.get() == currentScene) {
            currentScene = (it != scenes.begin()) ? std::prev(it)->second.get() : scenes.rbegin()->second.get();
            return;
        }
    }
}


void ScenesManager::replaceScene(const std::string& id, std::unique_ptr<Scene> scene) {
    bool wasCurrent = (scenes.count(id) && scenes[id].get() == currentScene);
    scenes[id] = std::move(scene);
    if (wasCurrent)
        currentScene = scenes[id].get();
}

void ScenesManager::removeScene(const std::string& id) {
    bool wasCurrent = (scenes.count(id) && scenes[id].get() == currentScene);
    scenes.erase(id);
    if (wasCurrent)
        currentScene = scenes.empty() ? nullptr : scenes.begin()->second.get();
}

void ScenesManager::addScene(std::unique_ptr<Scene> scene) {
    std::string id = scene->id;
    scenes[id] = std::move(scene);
    if (!currentScene)
        currentScene = scenes[id].get();
}