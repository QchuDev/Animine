#ifndef SCENE_H
#define SCENE_H

#include <map>
#include <vector>
#include <string>
#include "classes/entities/entity.h"
#include "classes/animations/animation.h"

class Scene {
public:
    std::string id;

    Scene(const std::string& id) : id(id) {}
    ~Scene();

    void addEntity(const std::string& entityId, IEntity* entity);
    void addAnimation(IAnimation* animation);

    IEntity* getEntity(const std::string& entityId) const;
    const std::map<std::string, IEntity*>& getEntities() const;
    const std::vector<IAnimation*>& getAnimations() const;

private:
    std::map<std::string, IEntity*> entities;
    std::vector<IAnimation*> animations;
};

#endif
