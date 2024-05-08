#pragma once
// #include "Movement.cpp"
#include "Camera.h"
#include <GLFW/glfw3.h>

// void processInput(GLFWwindow *window, Camera &camera, float deltaTime);

class Movement
{
public:
    void processInput(GLFWwindow *window, Camera *camera, float deltaTime);

private:
};