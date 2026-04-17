#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

class Camera {
    public:
        glm::vec3 Position;
        glm::vec3 Front;
        glm::vec3 Up;
        glm::vec3 Right;
        glm::vec3 WorldUp;

        // Ángulos de Euler
        float Yaw;
        float Pitch;

        // Opciones de cámara (opcional, pero útil)
        float MovementSpeed;
        float MouseSensitivity;
       
        Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f));
        
        // Returns the view matrix calculated through a lookAt
        glm::mat4 GetViewMatrix();
        
        // Process W,A,S,D movement
        void ProcessKeyboard(Camera_Movement direction, float deltaTime);
        void updateCameraVectors();       
        void ProcessMouseMovement(float xoffset, float yoffset);
        
};


#endif