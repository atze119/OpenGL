#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Shader.h"
#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <iostream>

// Magnifying means up-scaling, minifying means down-sclaing
// Magnifying cant be used on mipmaps because mipmaps are used to downscale the texture 

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
        // positions        // colors           // texture coords
        0.5f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   2.0f, 2.0f, // top-right
        0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,   2.0f, 0.0f, // bottom-right
       -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   0.0f, 0.0f, // bottom-left
       -0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 1.0f,   0.0f, 2.0f, // top-left
    };

    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3, // second triangle
    };

    // Vertex Buffer Object
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    // first triangle setup
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // position attribute
    // Parameters: layout (location=0), position = vec3, type of data, normalize data, stride, offset
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // color attribute, we need to set the offset!  offset = 3 positions in the vertex array. So we skip every 3 positions in the float array
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Unbind VertexArray
    glBindVertexArray(0);

    unsigned int texture1;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load and generate the texture
    // flip the y-axis of every image! Because image top coords are 0.0 and in openGL top coords 1.0
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrChannels;
    unsigned char* data = stbi_load("F:\\dev\\Visual Studio\\source\\repos\\OpenGL\\OpenGL\\src\\container.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    unsigned int texture2;
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    data = stbi_load("F:\\dev\\Visual Studio\\source\\repos\\OpenGL\\OpenGL\\src\\awesomeface.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
   

    ourShader.use();
    glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 0);
    ourShader.setInt("texture2", 1);

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

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        //ourShader.use();
        //ourShader.setFloat("positionOffset", 0.3f);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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