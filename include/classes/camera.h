#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
    public:
        glm::vec3 Position;
        glm::vec3 Front;
        glm::vec3 Up;
        
        float Yaw; // horizontal rotation
        float Pitch;// vertical rotation
        
        Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f));
        
        // Returns the view matrix calculated through a lookAt
        glm::mat4 GetViewMatrix();
        
        // Process W,A,S,D movement
        void ProcessKeyboard(const char* direction, float deltaTime);
            
    private:
};


#endif