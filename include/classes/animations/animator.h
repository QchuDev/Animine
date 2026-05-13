#ifndef ANIMATOR_H
#define ANIMATOR_H

class Scene;  // forward declaration

class Animator {
public:
    void update(float deltaTime, Scene* scene);
    void reset() { t = 0.0f; }

private:
    float t = 0.0f;
};

#endif
