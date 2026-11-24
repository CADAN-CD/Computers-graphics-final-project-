#ifndef KEYBOARD_INPUTS_H
#define KEYBOARD_INPUTS_H

#include <glm/glm.hpp>
#include "camera.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class KeyboardInputs
{
public:
    void processInput(
        GLFWwindow* window,
        Camera& camera,
        float deltaTime,
        bool captureMouse,
        bool captureKeyboard
    );
};

#endif
