#include "classes/animations/animator.h"
#include "classes/animations/animation.h"
#include "classes/animations/evaluate.h"
#include "classes/scenes/scene.h"

void Animator::update(float deltaTime, Scene* scene) {
    if (!scene) return;
    t += deltaTime;

    // First pass: apply all InstantSet commands that are due
    for (IAnimation* iAnim : scene->getAnimations()) {
        if (auto* set = dynamic_cast<InstantSet*>(iAnim)) {
            if (t >= set->startTime && !set->applied) {
                IEntity* e = scene->getEntity(set->entity_id);
                if (e) {
                    switch (set->property) {
                        case TransformProp::POSITION: e->setPosition(set->value); break;
                        case TransformProp::ROTATION: e->setRotation(set->value); break;
                        case TransformProp::SCALE:    e->setScale(set->value);    break;
                    }
                }
                set->applied = true;
            }
        }
    }

    // Second pass: process animations (captures happen after sets)
    for (IAnimation* iAnim : scene->getAnimations()) {
        Animation* anim = dynamic_cast<Animation*>(iAnim);
        if (!anim) continue;

        float localT = t - anim->startTime;
        if (localT < 0.0f || localT > anim->duration) continue;

        for (Track& track : anim->tracks) {
            IEntity* entity = scene->getEntity(track.entity_id);
            if (!entity) continue;

            if (!track.hasCaptured) {
                switch (track.property) {
                    case TransformProp::POSITION: track.capturedStart = entity->getPosition(); break;
                    case TransformProp::ROTATION: track.capturedStart = entity->getRotation(); break;
                    case TransformProp::SCALE:    track.capturedStart = entity->getScale();    break;
                }
                track.hasCaptured = true;
            }

            glm::vec3 value = evaluate(track, localT, anim->duration);

            switch (track.property) {
                case TransformProp::POSITION: entity->setPosition(value); break;
                case TransformProp::ROTATION: entity->setRotation(value); break;
                case TransformProp::SCALE:    entity->setScale(value);    break;
            }
        }
    }
}


void Animator::reset(Scene* scene) {
    t = 0.0f;
    if (!scene) return;

    // Restore all entities to their creation-time transform
    for (auto& [id, entity] : scene->getEntities())
        entity->restoreInitialTransform();

    // Reset animation state
    for (IAnimation* iAnim : scene->getAnimations()) {
        if (auto* set = dynamic_cast<InstantSet*>(iAnim)) {
            set->applied = false;
        } else if (auto* anim = dynamic_cast<Animation*>(iAnim)) {
            for (Track& track : anim->tracks)
                track.hasCaptured = false;
        }
    }
}
