#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 ourColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    // Multiplicamos la posición por las matrices (el orden importa: P * V * M)
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    ourColor = aColor;
}