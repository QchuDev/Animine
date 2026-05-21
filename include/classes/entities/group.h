#ifndef GROUP_H
#define GROUP_H

#include "classes/entities/entity.h"
#include <vector>
#include <string>

class Group : public IEntity {
public:
    std::vector<std::string> childIds;

    Group() : IEntity(nullptr) {}

    void draw(const glm::mat4& view, const glm::mat4& projection) override {
        (void)view; (void)projection;
        // Groups don't draw anything — they only provide a parent transform
    }
};

#endif
