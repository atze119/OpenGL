#include "stb_image.h" // This needs to be on top because of shitty include problems, stbi_image is included in Model.h!!!!
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

// Magnifying means up-scaling, minifying means down-sclaing
// Magnifying cant be used on mipmaps because mipmaps are used to downscale the texture 

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 0.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse{ true };

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) // glfwGetProcAddress defines the right function based on our OS
    {
        std::cout << "Couldn't load opengl" << std::endl;
        glfwTerminate();
        return -1;
    }

    // We can set the viewport to a different size, which means the renderering display will be a different size than the window
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell stbi_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(1);

    // Define shader
    Shader ourShader("Shader\\modelVertexShader.glsl", "Shader\\modelFragShader.glsl");
    Shader lightShader("Shader\\lightVertexShader.glsl", "Shader\\lightFragShader.glsl");

    // load model
    Model ourModel("resources\\models\\backpack.obj");
    // activating the shader before render because we dont change the activated shader in render loop (doesn't need to be reactivated every frame)
    ourShader.use();
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        /* Render here */
        // To enable z-buffer (depth)
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // this is a state setting function
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // this is a state using function

        // camera
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

  
        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(1.0f, 1.0f, -3.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        ourShader.setMat4("model", model);
        ourModel.Draw(ourShader);

        // User input
        processInput(window);
        /* Swap front and back buffers. Will swap the color buffers (2D), for each pixel in GLFW's window*/
        glfwSwapBuffers(window);
        /* Poll for and process events (user input)*/
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xPosIn, double yPosIn)
{
    float xPos = static_cast<float>(xPosIn);
    float yPos = static_cast<float>(yPosIn);

    if (firstMouse)
    {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }
    float xOffset = xPos - lastX;
    float yOffset = lastY - yPos; // reversed because y-coordinates go from bottom to top
    lastX = xPos;
    lastY = yPos;

    camera.ProcessMouseMovement(xOffset, yOffset);
}

void scroll_callback(GLFWwindow* window, double xOffset, double yOffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yOffset));
}

bool polygonModePressed = false;
void processInput(GLFWwindow* window)
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
            camera.ProcessKeyboard(FORWARD, deltaTime * sprintMultiplier);
        else if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            camera.ProcessKeyboard(FORWARD, deltaTime * sneakMultiplier);
        else
            camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            camera.ProcessKeyboard(BACKWARD, deltaTime * sprintMultiplier);
        else if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            camera.ProcessKeyboard(BACKWARD, deltaTime * sneakMultiplier);
        else
            camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            camera.ProcessKeyboard(LEFT, deltaTime * sprintMultiplier);
        else if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            camera.ProcessKeyboard(LEFT, deltaTime * sneakMultiplier);
        else
            camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            camera.ProcessKeyboard(RIGHT, deltaTime * sprintMultiplier);
        else if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            camera.ProcessKeyboard(RIGHT, deltaTime * sneakMultiplier);
        else
            camera.ProcessKeyboard(RIGHT, deltaTime);
    // light-movement
    // TODO: This needs to be implemented
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        // lightPos.x += 0.1f;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        //lightPos.x -= 0.1f;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        //lightPos.y += 0.1f;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        //lightPos.y -= 0.1f;
    if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS)
        if (glfwGetWindowAttrib(window, GLFW_MAXIMIZED))
            glfwRestoreWindow(window);
        else
            glfwMaximizeWindow(window);

}
