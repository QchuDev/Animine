#ifndef ANIMATOR_H
#define ANIMATOR_H

class Animator { 
    public:
        void update();
    private:
        float t = 0.0f;
        float maxT;
        bool isPlaying = false;
};

#endif