#ifndef ENTITY_H
#define ENTITY_H

/*
    This interface defines should be implemented by any object that 
    lives on the scene
*/
class IEntity {
    public:
        virtual ~IEntity();
    private:
        virtual void draw();
};


#endif 