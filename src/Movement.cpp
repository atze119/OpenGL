#include "Movement.h"
#include "Camera.h"
#include <GLFW/glfw3.h>

bool polygonModePressed = false;

void Movement::processInput(GLFWwindow *window, Camera *camera, float deltaTime)
{
    float sprintMultiplier = 3.0f;
    float sneakMultiplier = 0.5f;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE))
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        if (polygonModePressed == false)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            polygonModePressed = true;
            // TODO: add a cooldown timer
        }
        else
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            polygonModePressed = false;
        }
    }
    // Movements
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            camera->ProcessKeyboard(FORWARD, deltaTime * sprintMultiplier);
        else if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            camera->ProcessKeyboard(FORWARD, deltaTime * sneakMultiplier);
        else
            camera->ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            camera->ProcessKeyboard(BACKWARD, deltaTime * sprintMultiplier);
        else if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            camera->ProcessKeyboard(BACKWARD, deltaTime * sneakMultiplier);
        else
            camera->ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            camera->ProcessKeyboard(LEFT, deltaTime * sprintMultiplier);
        else if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            camera->ProcessKeyboard(LEFT, deltaTime * sneakMultiplier);
        else
            camera->ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            camera->ProcessKeyboard(RIGHT, deltaTime * sprintMultiplier);
        else if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            camera->ProcessKeyboard(RIGHT, deltaTime * sneakMultiplier);
        else
            camera->ProcessKeyboard(RIGHT, deltaTime);
    }
    // light-movement
    // TODO: This needs to be implemented
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        // lightPos.x += 0.1f;
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            // lightPos.x -= 0.1f;
            if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
                // lightPos.y += 0.1f;
                if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
                    // lightPos.y -= 0.1f;
                    if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS)
                    {
                        if (glfwGetWindowAttrib(window, GLFW_MAXIMIZED))
                            glfwRestoreWindow(window);
                        else
                            glfwMaximizeWindow(window);
                    }
}
