#include "Shader.h"
#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <iostream>

//const char* vertexShaderSource = "#version 460 core\n"
//"layout (location = 0) in vec3 aPos;\n"
//"layout (location = 1) in vec3 aColor;\n"
//"out vec3 ourColor;\n"
//"void main()\n"
//"{\n"
//"   gl_Position = vec4(aPos, 1.0f);\n"
//"   ourColor = aColor;\n"
//"}\0";
//
//const char* fragmentShaderSource = "#version 460 core\n"
//"out vec4 FragColor;\n"
//"in vec3 ourColor;\n"
//"void main()\n"
//"{\n"
//"   FragColor = vec4(ourColor, 1.0f);\n"
//"}\0";

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE))
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_SPACE))
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) // glfwGetProcAddress defines the right function based on our OS
    {
        std::cout << "Couldn't load opengl" << std::endl;
        glfwTerminate();
        return -1;
    }

    // We can set the viewport to a different size, which means the renderering display will be a different size than the window
    glViewport(0, 0, 800, 600);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // Needed to use absolute path... maybe change the linking in project or something. The file were searched in the wrong folder
    Shader ourShader("F:\\dev\\Visual Studio\\source\\repos\\OpenGL\\OpenGL\\src\\vertexShader.glsl", "F:\\dev\\Visual Studio\\source\\repos\\OpenGL\\OpenGL\\src\\fragmentShader.glsl");
 
    float vertices[] = {
        // positions        // colors
        0.0f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f, // top
       -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f, // bottom-left
        0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
    };

    // Vertex Buffer Object
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    // first triangle setup
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    // Parameters: layout (location=0), position = vec3, type of data, normalize data, stride, offset
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // color attribute, we need to set the offset!  offset = 3 positions in the vertex array. So we skip every 3 positions in the float array
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Unbind VertexArray
    glBindVertexArray(0);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // this is a state setting function
        glClear(GL_COLOR_BUFFER_BIT); // this is a state using function

        // Draw Triangle / Rectangle, use Program needs to be before we can search for the index of uniforms
        //ourShader.setFloat("someUniform", 1.0f);
        
        // update the uniform color
        //float timeValue = glfwGetTime();
        //float greenValue = sin(timeValue) / 2.0f + 0.5f; // change color over time, it varies the value between 0.0f - 1.0f
        //int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor"); // name of the variable to search for
        //glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f); // 4f == 4 float values as parameters

        ourShader.use();
        ourShader.setFloat("positionOffset", 0.3f);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // User input
        processInput(window);

        /* Swap front and back buffers. Will swap the color buffers (2D), for each pixel in GLFW's window*/
        glfwSwapBuffers(window);

        /* Poll for and process events (user input)*/
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}