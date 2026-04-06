#ifndef ENTITY_H
#define ENTITY_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // for matrix transformations lol

#include "classes/shader.h"


struct Transform
{
    glm::vec3 position = glm::vec3(0.0f);   
    glm::vec3 rotation = glm::vec3(0.0f);   // in degrees
    glm::vec3 scale = glm::vec3(1.0f);
    
    glm::mat4 getModelMatrix() const {
        glm::mat4 model = glm::mat4(1.0f);
        
        model = glm::translate(model, position);
        model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1, 0, 0));
        model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0, 1, 0));
        model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0, 0, 1));
        model = glm::scale(model, scale);
        
        return model;
    }
    

};

/*
    This interface defines should be implemented by any object that 
    lives on the scene
*/
class IEntity {
    public:
        Transform transform;
        virtual ~IEntity() {};// El "= 0" es OBLIGATORIO si no vas a dar una implementación aquí
        
        virtual void draw(Shader& shader, const glm::mat4& view, const glm::mat4& projection) = 0;
        
        // Getters/Setters
        virtual glm::vec3 getPosition() { return transform.position; }
        virtual void setPosition(glm::vec3 pos) { transform.position = pos; }
        virtual glm::vec3 getRotation() { return transform.rotation; }
        virtual void setRotation(glm::vec3 rot) { transform.rotation = rot; }
        virtual glm::vec3 getScale() { return transform.scale; }
        virtual void setScale(glm::vec3 sca) { transform.scale = sca; }
    };

#endif 