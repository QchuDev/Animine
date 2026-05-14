#ifndef ANIMATOR_H
#define ANIMATOR_H

class Scene;

class Animator {
public:
    void update(float deltaTime, Scene* scene);
    void reset(Scene* scene);

private:
    float t = 0.0f;
};

#endif
