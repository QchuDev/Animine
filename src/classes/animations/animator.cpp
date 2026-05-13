#include "classes/animations/animator.h"
#include "classes/animations/animation.h"
#include "classes/animations/evaluate.h"
#include "classes/scenes/scene.h"

void Animator::update(float deltaTime, Scene* scene) {
    if (!scene) return;
    t += deltaTime;

    for (IAnimation* iAnim : scene->getAnimations()) {
        Animation* anim = static_cast<Animation*>(iAnim);

        float localT = t - anim->startTime;
        if (localT < 0.0f || localT > anim->duration) continue;

        for (const Track& track : anim->tracks) {
            IEntity* entity = scene->getEntity(track.entity_id);
            if (!entity) continue;

            glm::vec3 value = evaluate(track, localT);

            switch (track.property) {
                case TransformProp::POSITION: entity->setPosition(value); break;
                case TransformProp::ROTATION: entity->setRotation(value); break;
                case TransformProp::SCALE:    entity->setScale(value);    break;
            }
        }
    }
}
