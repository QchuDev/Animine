#ifndef ENGINE_H
#define ENGINE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "classes/render/renderer.h"
#include "classes/animations/animator.h"

/*
    In this file we define the atributtes, methods, public and private of the class.
    The interface of the class is here, everything we have and can do.
    
    NOT HOW, BUT WHAT
*/

class Engine {
    public:
        Engine();   // Constructor of the class
        ~Engine();  // Destructor of the class -> to free memory
        bool init(int width, int height, const char* title);
        void run(); // Method to start the main loop
        double lastX = 640.0, lastY = 360.0; // Inicializar en el centro de tu ventana
        bool firstMouse = true;
    private:
        GLFWwindow* window;
        
        Renderer* renderer; // Instance of the one who draws on screen
        Animator* animator; // Instance of the one who transforms the entities

        float deltaTime = 0.0f;
        float lastFrame = 0.0f;
        void processInput(); // Listens to the the user inputs 
        
};

#endif

