#define GLM_ENABLE_EXPERIMENTAL

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/noise.hpp>
#include <glm/gtx/transform2.hpp>
#include <glm/gtx/string_cast.hpp>

#include "../include/shader.h"
#include "../include/camera.h"
#include "../include/texture.h"
#include "../include/VertexBuffer.h"
#include "../include/Vertices.h"
#include <vector>

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

void processInput(GLFWwindow* window); // for continous key press
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods); // single key presses, i.e toggles
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
unsigned int loadCubemap(std::vector<std::string> faces);


int resWidth = WINDOW_WIDTH;
int resHeight = WINDOW_HEIGHT;

float lastX = (float)resWidth / 2.0;
float lastY = (float)resHeight / 2.0;
bool firstMouse = true;

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

bool mouseToggle = true;
bool enableLighting = true;


Camera camera;
int main()
{
        GLFWwindow* window;

        /* Initialize the library */
        if (!glfwInit())
            return -1;

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        const char* glsl_version = "#version 460";
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_SAMPLES, 16); // MSAA 16x
        glfwSwapInterval(0);

        /* Create a windowed mode window and its OpenGL context */
        window = glfwCreateWindow(resWidth, resHeight, "OpenGL Grass", NULL, NULL);
        if (!window)
        {
            glfwTerminate();
            return -1;
        }

        /* Make the window's context current */
        glfwMakeContextCurrent(window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return -1;
        }

        // CALLBACKS
        glfwSetKeyCallback(window, key_callback);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSetCursorPosCallback(window, mouse_callback);
        //--------------------------------------

        // LOCK AND HIDE MOUSE (disabled since the demo start in orbit mode)
        //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        glViewport(0, 0, resWidth, resHeight);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);
        ImGuiIO& io = ImGui::GetIO(); (void)io;

        Shader skyboxShader("assets/shaders/skybox.vert", "assets/shaders/skybox.frag");
        Shader shader("assets/shaders/shader.vert", "assets/shaders/shader.frag");
        shader.use();

        glm::vec3 dirLightDirection = glm::vec3(-0.9f, -1.5f, -1.9f);
        shader.setVec3("dirLight.direction", dirLightDirection);
        shader.setVec3("dirLight.ambient", 0.7f, 0.5f, 0.2f);
        shader.setVec3("dirLight.diffuse", 0.9f, 0.7f, 0.2f);
        shader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
        shader.setInt("material.diffuse", 0);

        shader.use();
        Texture grassTexture("assets/textures/grass6.png", 0);
        grassTexture.bind();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        grassTexture.unbind();

        Texture groundTexture("assets/textures/dirt.png", 0);
        Texture containerTexture("assets/textures/container.jpg", 0);

        std::vector<std::string> faces {
            "assets/textures/skybox/right.jpg",
            "assets/textures/skybox/left.jpg",
            "assets/textures/skybox/top.jpg",
            "assets/textures/skybox/bottom.jpg",
            "assets/textures/skybox/front.jpg",
            "assets/textures/skybox/back.jpg"
        };
        unsigned int skyboxTextureID = loadCubemap(faces);
        skyboxShader.use();
        skyboxShader.setInt("skybox", 0);

        shader.use();

        // crate offsets for grass intanses
        const int grassAmmount = 70000;
        glm::vec3* translations = new glm::vec3[grassAmmount];
        int index = 0;
        float offset = 0.01f;
        for (int y = -200; y < 200; y += 2)
        {
            for (int x = -200; x < 200; x += 2)
            {
                float xzSNoise = glm::simplex(glm::vec2((float)x / 3.0f, (float)y / 3.0f));
                float ySNoise = glm::simplex(glm::vec2((float)x, (float)y) / 100.0f);
                glm::vec3 translation;
                translation.x = (float)x / 8.0f + xzSNoise;
                translation.y = ySNoise * 0.7;
                translation.z = (float)y / 8.0f + xzSNoise;
                translations[index++] = translation;
            }
        }
        VertexBuffer instanceVBO(translations, grassAmmount * sizeof(glm::vec3));

        // grass buffer
        unsigned int grassVAO;
        VertexBuffer grassVBO(grassVertices, sizeof(grassVertices));
        glGenVertexArrays(1, &grassVAO);
        glBindVertexArray(grassVAO);
        grassVBO.bind();
        glBufferData(GL_ARRAY_BUFFER, sizeof(grassVertices), grassVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        instanceVBO.bind();
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        instanceVBO.unbind();
        glVertexAttribDivisor(2, 1);

        grassVBO.bind();
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glBindVertexArray(0);

        // ground buffer
        unsigned int groundVAO;
        VertexBuffer groundVBO(groundVertices, sizeof(groundVertices));
        glGenVertexArrays(1, &groundVAO);
        glBindVertexArray(groundVAO);
        groundVBO.bind();
        glBufferData(GL_ARRAY_BUFFER, sizeof(groundVertices), groundVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glBindVertexArray(0);

        // container buffer
        unsigned int containerVAO;
        VertexBuffer containerVBO(containerVertices, sizeof(containerVertices));
        glGenVertexArrays(1, &containerVAO);
        glBindVertexArray(containerVAO);
        containerVBO.bind();
        glBufferData(GL_ARRAY_BUFFER, sizeof(containerVertices), containerVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glBindVertexArray(0);

        // skybox vertex buffer
        unsigned int skyboxVAO;
        VertexBuffer skyboxVBO(skyboxVertices, sizeof(skyboxVertices));
        glGenVertexArrays(1, &skyboxVAO);
        glBindVertexArray(skyboxVAO);
        skyboxVBO.bind();
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glBindVertexArray(0);

        glEnable(GL_MULTISAMPLE); // MSAA Enable
        glEnable(GL_DEPTH_TEST);
        while (!glfwWindowShouldClose(window))
        {
            processInput(window);

            float currentFrame = glfwGetTime();
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            glClearColor(0.678f, 0.847f, 0.902f, 1.0f);
            glClearDepth(1);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            shader.use();
            
            glm::mat4 view = camera.getViewMatrix();
            //camera.cameraPos = glm::vec3(13.0f * (float)sin(50.0 * 0.01f), 0.8f, -12.0f * (float)cos(50.0 * 0.01f));
            //camera.cameraFront =- camera.cameraPos;


            // make camera orbit
            camera.cameraPos = glm::vec3(13.0f * (float)sin(glfwGetTime() * 0.01f), 0.8f, -12.0f * (float)cos(glfwGetTime() * 0.01f));
            camera.cameraFront =- camera.cameraPos;


            glm::mat4 projection = glm::perspective(glm::radians(camera.fov), (float)resWidth / float(resHeight), 0.1f, 100.0f);
            glm::mat4 viewProj = projection * view;
            shader.setMat4("viewProj", viewProj);

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::scale(model, glm::vec3(0.5));
            model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f));
            shader.setMat4("model", model);

            float sNoise = glm::simplex(glm::vec2((float)glfwGetTime() / 3.0f, (float)glfwGetTime() / 8.0f));
            float shearAngleFast = 0.2f * sNoise * (float)sin(sin(glfwGetTime()));
            glm::mat4 shearFast = glm::mat4(1.0f);
            shearFast = glm::translate(shearFast, glm::vec3(0.5 * shearAngleFast, 0.0f, 0.0f));
            shearFast = glm::shearY3D(shearFast, shearAngleFast, 0.0f);
            shader.setMat4("shearFast", shearFast);

            float shearAngleSlow = 0.2f * sNoise * (float)sin(sin(glfwGetTime() + 0.3f));
            glm::mat4 shearSlow = glm::mat4(1.0f);
            shearSlow = glm::translate(shearSlow, glm::vec3(0.5 * shearAngleSlow, 0.0f, 0.0f));
            shearSlow = glm::shearY3D(shearSlow, shearAngleSlow, 0.0f);
            shader.setMat4("shearSlow", shearSlow);

            glActiveTexture(GL_TEXTURE0);
            glBindVertexArray(grassVAO);
            grassTexture.bind();
            //glDrawArrays(GL_TRIANGLES, 0, 18);
            glDrawArraysInstanced(GL_TRIANGLES, 0, 18, grassAmmount);

            model = glm::mat4(1.0f);
            shader.setMat4("model", model);
            glBindVertexArray(groundVAO);
            groundTexture.bind();
            glDrawArrays(GL_TRIANGLES, 0, 6);

            glBindVertexArray(containerVAO);

            shader.setVec3("viewPos", camera.cameraPos);
            shader.setFloat("material.shininess", 32.0f);

            model = glm::scale(model, glm::vec3(1.0f));
            model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.5f));
            shader.setMat4("model", model);
            containerTexture.bind();
            glDrawArrays(GL_TRIANGLES, 0, 36);

            // skybox
            glDepthFunc(GL_LEQUAL);
            skyboxShader.use();
            skyboxShader.setMat4("projection", projection);
            view = glm::mat4(glm::mat3(camera.getViewMatrix())); // removes the translations from the view matrix
            skyboxShader.setMat4("view", view);
            glBindVertexArray(skyboxVAO);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextureID);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glDepthFunc(GL_LESS);

            shader.use();
            shader.setBool("enableLighting", enableLighting);

            // Start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            {
                ImGui::Begin("Debug Menu");
                ImGui::Text("Sunlight Direction Vector");
                ImGui::SliderFloat3("XYZ", glm::value_ptr(dirLightDirection), -10.0f, 0.0f);
                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
                ImGui::End();
            }
            //dirLightDirection.x = sin(glfwGetTime()) * 5.0;
            //dirLightDirection.z = -cos(glfwGetTime()) * 5.0;
            shader.setVec3("dirLight.direction", dirLightDirection);

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            glfwSwapBuffers(window);

            glfwPollEvents();
        }

        glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.keyboardMovement(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.keyboardMovement(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.keyboardMovement(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.keyboardMovement(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.keyboardMovement(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        camera.keyboardMovement(DOWN, deltaTime);
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    resWidth = width;
    resHeight = height;
    glViewport(0, 0, width, height);
}
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{

    if (!mouseToggle)
    {
        camera.mouseMovement(&lastX, &lastY, xposIn, yposIn);
    }
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_M && action == GLFW_PRESS) mouseToggle = !mouseToggle;
    if (mouseToggle) glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    else glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (key == GLFW_KEY_L && action == GLFW_PRESS) enableLighting = !enableLighting;


}
unsigned int loadCubemap(std::vector<std::string> faces)
{

    stbi_set_flip_vertically_on_load(false);

    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            // we can iterate through all six texture targets (sides of the cube) with an incrementing int since GL_TEXTURE_CUBE_MAP_POSITIVE_X is just an enum
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}
