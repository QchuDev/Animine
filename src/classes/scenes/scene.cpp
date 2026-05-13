#include "classes/scenes/scene.h"

void Scene::addEntity(const std::string& entityId, IEntity* entity) {
    entities[entityId] = entity;
}

void Scene::addAnimation(IAnimation* animation) {
    animations.push_back(animation);
}

const std::map<std::string, IEntity*>& Scene::getEntities() const {
    return entities;
}

IEntity* Scene::getEntity(const std::string& entityId) const {
    auto it = entities.find(entityId);
    return it != entities.end() ? it->second : nullptr;
}

const std::vector<IAnimation*>& Scene::getAnimations() const {
    return animations;
}

Scene::~Scene() {
    for (auto& [id, entity] : entities) delete entity;
    for (auto* anim : animations) delete anim;
}
