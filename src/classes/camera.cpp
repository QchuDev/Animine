#include "classes/camera.h"
#include <glm/glm.hpp>
#include <iostream>

Camera::Camera(glm::vec3 position) {
    Position = position;
    WorldUp = glm::vec3(0.0f, 1.0f, 0.0f); // El "arriba" del mundo siempre es Y+
    Yaw = -90.0f;  // Apuntando hacia el frente inicial (Z negativo)
    Pitch = 0.0f;
    
    updateCameraVectors(); // Calculamos Front, Right y Up por primera vez
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
    float velocity = 2.0f * deltaTime; 
    
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
        
}


void Camera::updateCameraVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    
    Front = glm::normalize(front);
    Right = glm::normalize(glm::cross(Front, WorldUp)); 
    Up    = glm::normalize(glm::cross(Right, Front));
    
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset) {
    float sensitivity = 0.05f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    Yaw   += xoffset;
    Pitch += yoffset;

    // Restricción para no "rompernos el cuello" mirando atrás
    if (Pitch > 89.0f)  Pitch = 89.0f;
    if (Pitch < -89.0f) Pitch = -89.0f;

    // ¡Importante! Recalcular los vectores Front, Right y Up
    updateCameraVectors();
}