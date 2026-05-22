#ifndef SCENES_MANAGER_H
#define SCENES_MANAGER_H

#include <map>
#include <string>
#include <vector>
#include <memory>
#include "classes/scenes/scene.h"

class ScenesManager {
public:
    ScenesManager(std::vector<std::unique_ptr<Scene>> scenes);

    Scene* getCurrentScene() const;
    void setCurrentScene(const std::string& id);
    void nextScene();
    void prevScene();
    void replaceScene(const std::string& id, std::unique_ptr<Scene> scene);
    void removeScene(const std::string& id);
    void addScene(std::unique_ptr<Scene> scene);

private:
    std::map<std::string, std::unique_ptr<Scene>> scenes;
    Scene* currentScene = nullptr;
};

#endif
