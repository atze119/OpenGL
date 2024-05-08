#include <glad/glad.h>
#include "stb_image.h" // This needs to be on top because of shitty include problems, stbi_image is included in Model.h!!!!
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "filesystem.h"
#include "Movement.h"
// #include "Texture.h"
#include <GLFW/glfw3.h>
#include <glm/gtx/norm.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
// #include <format>
#include <map>

// Magnifying means up-scaling, minifying means down-sclaing
// Magnifying cant be used on mipmaps because mipmaps are used to downscale the texture

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
unsigned int loadTexture(char const *path);
unsigned int loadCubemap(std::vector<std::string> faces);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 0.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse{true};

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main(void)
{
    GLFWwindow *window;

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
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell stbi_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(false);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //// face-culling
    // glEnable(GL_CULL_FACE);
    // glFrontFace(GL_CW);
    // glCullFace(GL_BACK);

    Shader shader = Shader(FileSystem::getPath("shader/depthTesting/depthTestingVert.glsl").c_str(), FileSystem::getPath("shader/depthTesting/depthTestingFrag.glsl").c_str());
    Shader screenShader = Shader(FileSystem::getPath("shader/bufferTesting/bufferTestingVert.glsl").c_str(), FileSystem::getPath("shader/bufferTesting/bufferTestingFrag.glsl").c_str());

    // set up vertex data (and buffer(s)) and configure vertex attributes. Now clockwise and counter-clockwise for face-culling
    // ------------------------------------------------------------------
    float cubeVertices[] = {
        // Back face
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // Bottom-left
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f,  // bottom-right
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,   // top-right
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,   // top-right
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,  // top-left
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // bottom-left
        // Front face
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // bottom-left
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,   // top-right
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,  // bottom-right
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,   // top-right
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // bottom-left
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,  // top-left
        // Left face
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,   // top-right
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // bottom-left
        -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,  // top-left
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // bottom-left
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,   // top-right
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,  // bottom-right
                                         // Right face
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,    // top-left
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,   // top-right
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,  // bottom-right
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,  // bottom-right
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f,   // bottom-left
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,    // top-left
        // Bottom face
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // top-right
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,   // bottom-left
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f,  // top-left
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,   // bottom-left
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // top-right
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,  // bottom-right
        // Top face
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, // top-left
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,  // top-right
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,   // bottom-right
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,   // bottom-right
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,  // bottom-left
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f  // top-left
    };
    float planeVertices[] = {
        // positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
        5.0f, -0.5f, 5.0f, 2.0f, 0.0f,
        -5.0f, -0.5f, 5.0f, 0.0f, 0.0f,
        -5.0f, -0.5f, -5.0f, 0.0f, 2.0f,

        5.0f, -0.5f, 5.0f, 2.0f, 0.0f,
        -5.0f, -0.5f, -5.0f, 0.0f, 2.0f,
        5.0f, -0.5f, -5.0f, 2.0f, 2.0f};

    float windowVertices[] = {
        // positions         // texture Coords (flipped y coordinates because texture is flipped upside down)
        0.0f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.0f, -0.5f, 0.0f, 0.0f, 0.0f,
        1.0f, -0.5f, 0.0f, 1.0f, 0.0f,

        0.0f, 0.5f, 0.0f, 0.0f, 1.0f,
        1.0f, -0.5f, 0.0f, 1.0f, 0.0f,
        1.0f, 0.5f, 0.0f, 1.0f, 1.0f};

    float screenVertices[] = {// vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
                              // positions   // texCoords
                              -0.3f, 1.0f, 0.0f, 1.0f,
                              -0.3f, 0.7f, 0.0f, 0.0f,
                              0.3f, 0.7f, 1.0f, 0.0f,

                              -0.3f, 1.0f, 0.0f, 1.0f,
                              0.3f, 0.7f, 1.0f, 0.0f,
                              0.3f, 1.0f, 1.0f, 1.0f};

    std::vector<glm::vec3> windows;
    windows.push_back(glm::vec3(-1.5f, 0.0f, -0.48f));
    windows.push_back(glm::vec3(1.5f, 0.0f, 0.51f));
    windows.push_back(glm::vec3(0.0f, 0.0f, 0.7f));
    windows.push_back(glm::vec3(-0.3f, 0.0f, -2.3f));
    windows.push_back(glm::vec3(0.5f, 0.0f, -0.6f));

    // cube buffers
    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glBindVertexArray(0);

    // floor buffers
    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glBindVertexArray(0);

    // window
    unsigned int windowVAO, windowVBO;
    glGenVertexArrays(1, &windowVAO);
    glGenBuffers(1, &windowVBO);
    glBindVertexArray(windowVBO);
    glBindBuffer(GL_ARRAY_BUFFER, windowVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(windowVertices), &windowVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glBindVertexArray(0);

    // screen-quad buffer
    unsigned int screenVAO, screenVBO;
    glGenVertexArrays(1, &screenVAO);
    glGenBuffers(1, &screenVBO);
    glBindVertexArray(screenVAO);
    glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(screenVertices), &screenVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    glBindVertexArray(0);

    // load textures
    unsigned int cubeTexture = loadTexture(FileSystem::getPath("resources/textures/container.jpg").c_str());
    unsigned int planeTexture = loadTexture(FileSystem::getPath("resources/textures/metal.png").c_str());
    unsigned int windowTexture = loadTexture(FileSystem::getPath("resources/textures/blending_transparent_window.png").c_str());

    // load model
    Model backPackModel(FileSystem::getPath("resources/models/backpack/backpack.obj"));

    // load cubemap
    std::vector<std::string> faces{
        FileSystem::getPath("resources/textures/skybox/right.jpg").c_str(),
        FileSystem::getPath("resources/textures/skybox/left.jpg").c_str(),
        FileSystem::getPath("resources/textures/skybox/top.jpg").c_str(),
        FileSystem::getPath("resources/textures/skybox/bottom.jpg").c_str(),
        FileSystem::getPath("resources/textures/skybox/front.jpg").c_str(),
        FileSystem::getPath("resources/textures/skybox/back.jpg").c_str()};
    unsigned int cubeMapTexture = loadCubemap(faces);

    float skyboxVertices[] = {
        // positions
        -1.0f, 1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        -1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f};

    Shader skyboxShader = Shader(FileSystem::getPath("shader/cubemap/cubemapVert.glsl").c_str(), FileSystem::getPath("shader/cubemap/cubemapFrag.glsl").c_str());

    // cubemap VAO, VBO
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    // glBindVertexArray(0);

    // shader configuration
    shader.use();
    shader.setInt("texture1", 0);

    Shader oneColorShader(FileSystem::getPath("shader/depthTesting/depthTestingVert.glsl").c_str(), FileSystem::getPath("shader/shaderSingleColor.glsl").c_str());

    screenShader.use();
    screenShader.setInt("screenTexture", 0);

    Shader modelShader(FileSystem::getPath("shader/model/modelVertexShader.glsl").c_str(), FileSystem::getPath("shader/model/modelFragShader.glsl").c_str());

    unsigned int framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    // generate texture
    unsigned int textureColorBuffer;
    glGenTextures(1, &textureColorBuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // attach to current framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer, 0);
    // glBindTexture(GL_TEXTURE_2D, 0);

    // create renderbuffer for depth and stencil testing
    // allocating buffer memory
    unsigned int renderbuffer;
    glGenRenderbuffers(1, &renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
    // unbind buffer after allocating enough storage
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // attach renderbuffer to framebuffer
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    // unbind Framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    Movement movement = Movement();

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        movement.processInput(window, &camera, deltaTime);

        // render
        // ------

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glEnable(GL_DEPTH_TEST);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // don't forget to clear the stencil buffer!

        // 1st. render pass is drawing mirror texture
        // set uniforms
        oneColorShader.use();
        glm::mat4 model = glm::mat4(1.0f);
        camera.Yaw += 180.0f;
        camera.ProcessMouseMovement(0, 0, false);
        glm::mat4 view = camera.GetViewMatrix();
        camera.Yaw -= 180.0f;
        camera.ProcessMouseMovement(0, 0, true);
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        oneColorShader.setMat4("view", view);
        oneColorShader.setMat4("projection", projection);

        shader.use();
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);

        // 1st. render pass, draw mirror texture
        // --------------------------------------------------------------------
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);
        // cubes
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(cubeVAO);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);
        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        // floor
        // draw floor normal
        glStencilMask(0x00);

        glBindVertexArray(planeVAO);
        glBindTexture(GL_TEXTURE_2D, planeTexture);
        model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(1.5, 1.5, 1.0));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        // To render each window from farthes to nearest
        std::map<float, glm::vec3> sorted;
        for (unsigned int i = 0; i < windows.size(); i++)
        {
            // length2 greater performance than glm::length
            float distance = glm::length2(camera.Position - windows[i]);
            sorted[distance] = windows[i];
        }
        glBindVertexArray(windowVAO);
        glBindTexture(GL_TEXTURE_2D, windowTexture);
        for (std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, it->second);
            shader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        // 2nd. render pass: draw as normal
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        model = glm::mat4(1.0f);
        view = camera.GetViewMatrix();
        shader.setMat4("view", view);

        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);
        // cubes
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(cubeVAO);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);
        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        // floor
        // draw floor normal
        glStencilMask(0x00);

        glBindVertexArray(planeVAO);
        glBindTexture(GL_TEXTURE_2D, planeTexture);
        model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(1.5, 1.5, 1.0));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        // To render each window from farthes to nearest
        for (unsigned int i = 0; i < windows.size(); i++)
        {
            // length2 greater performance than glm::length
            float distance = glm::length2(camera.Position - windows[i]);
            sorted[distance] = windows[i];
        }
        glBindVertexArray(windowVAO);
        glBindTexture(GL_TEXTURE_2D, windowTexture);
        for (std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, it->second);
            shader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        // bind framebuffer back to default
        // glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);
        // glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        screenShader.use();
        glBindVertexArray(screenVAO);
        glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
            screenShader.setInt("visionEffect", 0);
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
            screenShader.setInt("visionEffect", 1);
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
            screenShader.setInt("visionEffect", 2);
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
            screenShader.setInt("visionEffect", 3);

        glEnable(GL_DEPTH_TEST);

        // glDepthMask(GL_FALSE);
        // draw skybox as last
        glDepthFunc(GL_LEQUAL); // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default

        // 2nd. render pass: now draw slightly scaled versions of the objects, this time disabling stencil writing.
        // Because the stencil buffer is now filled with several 1s. The parts of the buffer that are 1 are not drawn, thus only drawing
        // the objects' size differences, making it look like borders.
        // -----------------------------------------------------------------------------------------------------------------------------
        // glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        // glStencilMask(0x00);
        // glDisable(GL_DEPTH_TEST);
        // oneColorShader.use();
        // float scale = 1.1f;
        //// cubes
        // glBindVertexArray(cubeVAO);
        // glBindTexture(GL_TEXTURE_2D, cubeTexture);
        // model = glm::mat4(1.0f);
        // model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
        // model = glm::scale(model, glm::vec3(scale, scale, scale));
        // oneColorShader.setMat4("model", model);
        // glDrawArrays(GL_TRIANGLES, 0, 36);
        // model = glm::mat4(1.0f);
        // model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
        // model = glm::scale(model, glm::vec3(scale, scale, scale));
        // oneColorShader.setMat4("model", model);
        // glDrawArrays(GL_TRIANGLES, 0, 36);
        // glBindVertexArray(0);
        // glStencilMask(0xFF);
        // glStencilFunc(GL_ALWAYS, 0, 0xFF);
        // glEnable(GL_DEPTH_TEST);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteVertexArrays(1, &screenVAO);
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &planeVBO);
    glDeleteBuffers(1, &screenVBO);
    glDeleteBuffers(1, &skyboxVBO);
    glDeleteRenderbuffers(1, &renderbuffer);
    glDeleteFramebuffers(1, &framebuffer);

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow *window, double xPosIn, double yPosIn)
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

void scroll_callback(GLFWwindow *window, double xOffset, double yOffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yOffset));
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const *path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format = 0;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        if (format == GL_RGBA)
        {
            // prevents generating a small stripe on textures with alpha (when drawing transparent)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }
        else
        {
            // normal configuration
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        }
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

unsigned int loadCubemap(std::vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
        }
        stbi_image_free(data);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}
