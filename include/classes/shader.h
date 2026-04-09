#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
    public:
        unsigned int ID; // The ID is the program shaders in the GPU
        
        // Guardamos las locaciones aquí
        int modelLoc;
        int viewLoc;
        int projLoc;
        
        Shader(const char* vertexPath, const char* fragmentPath);
        
        void use(); // activates the shader
        
        // To send data to the GPU
        void setMat4(int location, const glm::mat4& mat)const {
            glUniformMatrix4fv(location, 1, GL_FALSE, &mat[0][0]);
        }
        
        void setInt(const std::string& name, int value) const {
            // we search for name in the program
            int location = glGetUniformLocation(this->ID, name.c_str());
            
            // check and assign the value
            if (location != -1) {
                glUniform1i(location, value);
            } else {
                std::cerr << "Uniform '" << name << "' not found!" << std::endl;
            }
        }
        
    private:
        void checkCompileErrors(unsigned int shader, std::string type);    
};


#endif