#ifndef SCENES_MANAGER_H
#define SCENES_MANAGER_H

#include <map>
#include <string>
#include <vector>
#include <memory>
#include "classes/scenes/scene.h"

class ScenesManager {
public:
    // Takes ownership of all scenes. Sets current to the first one.
    ScenesManager(std::vector<std::unique_ptr<Scene>> scenes);

    Scene* getCurrentScene() const;
    void setCurrentScene(const std::string& id);

private:
    std::map<std::string, std::unique_ptr<Scene>> scenes;
    Scene* currentScene = nullptr;
};

#endif
