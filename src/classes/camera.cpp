#include "classes/camera.h"

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
void Camera::ProcessKeyboard(const char* direction, float deltaTime) {
    float velocity = 2.5f * deltaTime;
    if (direction == "FORWARD")  Position += Front * velocity;
    if (direction == "BACKWARD") Position -= Front * velocity;
    if (direction == "LEFT")     Position -= glm::normalize(glm::cross(Front, Up)) * velocity;
    if (direction == "RIGHT")    Position += glm::normalize(glm::cross(Front, Up)) * velocity;
}

