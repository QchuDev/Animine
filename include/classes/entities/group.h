#ifndef GROUP_H
#define GROUP_H

#include "classes/entities/entity.h"
#include <vector>
#include <string>
#include <map>

class Group : public IEntity {
public:
    std::vector<std::string> childIds;
    std::map<std::string, IEntity*>* entitiesRef = nullptr; // set by parser after adding to scene

    Group() : IEntity(nullptr) {}

    void draw(const glm::mat4& view, const glm::mat4& projection) override {
        (void)view; (void)projection;
    }

    void setColor(glm::vec3 c) override {
        if (!entitiesRef) return;
        for (auto& childId : childIds) {
            auto it = entitiesRef->find(childId);
            if (it != entitiesRef->end()) it->second->setColor(c);
        }
    }

    glm::vec3 getColor() override {
        if (!entitiesRef || childIds.empty()) return glm::vec3(1.0f);
        auto it = entitiesRef->find(childIds[0]);
        return (it != entitiesRef->end()) ? it->second->getColor() : glm::vec3(1.0f);
    }
};

#endif
