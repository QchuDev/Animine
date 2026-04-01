#include "classes/camera.h"
#include <iostream>

Camera::Camera(glm::vec3 position) : Position(position)  {
    Front = glm::vec3(0.0f, 0.0f, -1.0f);
    Up    = glm::vec3(0.0f, 1.0f, 0.0f);
    Yaw   = -90.0f;
    Pitch = 0.0f;
}

/**
 * Returns the ViewMatrix of the camera
 */
glm::mat4 Camera::GetViewMatrix() {
    return glm::lookAt(Position, Position + Front, Up);
}

/**
 * Proccesses the W,A,S,D movement of the camera 
 */
void Camera::ProcessKeyboard(const Camera_Movement direction, float deltaTime) {
    float velocity = 5.0f * deltaTime; 
    
    if (direction == FORWARD)
        Position += Front * velocity;
    if (direction == BACKWARD)
        Position -= Front * velocity;
    if (direction == LEFT)
        Position -= glm::normalize(glm::cross(Front, Up)) * velocity;
    if (direction == RIGHT)
        Position += glm::normalize(glm::cross(Front, Up)) * velocity;
    if (direction == UP)
        Position += glm::normalize(Up) * velocity;
    if (direction == DOWN)
        Position -= glm::normalize(Up) * velocity;
        
    std::cout << "Velocidad calculada: " << velocity << " Pos Z: " << Position.z << std::endl;
}

