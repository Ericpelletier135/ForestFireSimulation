#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <chrono>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <GLFW/glfw3.h>
#include "FireRenderer.h"
#include "FireSimulator.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// Camera/Mouse movement variables
float lastX = 400, lastY = 300;
bool firstMouse = true;
glm::vec3 cameraPos = glm::vec3(-20.0f, 30.0f, -20.0f);
glm::vec3 cameraFront = glm::normalize(-cameraPos);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.2f, 0.0f);
float cameraDistance = glm::length(cameraPos);
float yaw = -90.0f, pitch = -45.0f;

// simulation variables
bool simulationPaused = false;
glm::vec3 windDirection; 
float windIntensity; 
float humidity ; 
float diffusionRate;
float burningRate;
float ignitionRate;
float coolingRate;
float temperatureIncreaseRate;

float k1;
float burningRate_W_Humidity;
float k2;
float ignitionRate_W_Humidity;

int voxelGridWidth;
int voxelGridHeight;
int voxelGridDepth;

FireTerrain fireTerrain;
FireSimulator* fireSimulator;
FireRenderer* fireRenderer;

std::chrono::high_resolution_clock::time_point startTime;
std::chrono::high_resolution_clock::time_point endTimeBurn;
std::chrono::high_resolution_clock::time_point endTimeIgnite;
std::chrono::high_resolution_clock::time_point midTime;
float elapsedTime;
bool burning;
bool firstBurn;
float elapsedTimeIgnited;
bool allIgnited;
bool firstIgnite;

// Mouse callback function
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.5f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f) {
        pitch = 89.0f;
    }
    if (pitch < -89.0f) {
        pitch = -89.0f;
    }

    glm::vec3 front;
    front.x = cameraDistance * cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = cameraDistance * sin(glm::radians(pitch));
    front.z = cameraDistance * sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraPos = glm::normalize(front) * cameraDistance;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        lastX = xpos;
        lastY = ypos;
        firstMouse = true;
        glfwSetCursorPosCallback(window, mouse_callback);
    } else {
        glfwSetCursorPosCallback(window, nullptr);
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        float delta = 0.1f;

        if (mods & GLFW_MOD_SHIFT) {
            switch (key) {
                case GLFW_KEY_X:
                    windDirection.x -= delta;
                    break;
                case GLFW_KEY_Y:
                    windDirection.y -= delta;
                    break;
                case GLFW_KEY_Z:
                    windDirection.z -= delta;
                    break;
                case GLFW_KEY_W:
                    windIntensity -= delta;
                    break;
                case GLFW_KEY_H:
                    humidity -= delta;
                    burningRate_W_Humidity = burningRate * std::exp(-humidity * k1);
                    ignitionRate_W_Humidity = ignitionRate * std::exp(-humidity * k2); 
                    break;
                case GLFW_KEY_D:
                    diffusionRate -= delta;
                    break;
                case GLFW_KEY_B:
                    burningRate -= delta;
                    burningRate_W_Humidity = burningRate * std::exp(-humidity * k1);
                    break;
                case GLFW_KEY_I:
                    ignitionRate -= delta;
                    ignitionRate_W_Humidity = ignitionRate * std::exp(-humidity * k2); 
                    break;
                case GLFW_KEY_C:
                    coolingRate -= delta;
                    break;
                case GLFW_KEY_F:
                    temperatureIncreaseRate -= 10.0f;
                    break;
            }
        } else {

            switch (key) {
                case GLFW_KEY_LEFT_SHIFT || GLFW_KEY_RIGHT_SHIFT:
                    break;
                case GLFW_KEY_SPACE:
                    simulationPaused = !simulationPaused;
                    break;
                case GLFW_KEY_R:
                    fireSimulator = new FireSimulator(voxelGridWidth, voxelGridHeight, voxelGridDepth, fireTerrain); 
                    fireRenderer = new FireRenderer(*fireSimulator);
                    burning = true;
                    firstBurn = true;
                    allIgnited = false;
                    firstIgnite = true;
                    startTime = std::chrono::high_resolution_clock::now();
                    break;
                case GLFW_KEY_X:
                    windDirection.x += delta;
                    break;
                case GLFW_KEY_Y:
                    windDirection.y += delta;
                    break;
                case GLFW_KEY_Z:
                    windDirection.z += delta;
                    break;
                case GLFW_KEY_W:
                    windIntensity += delta;
                    break;
                case GLFW_KEY_H:
                    humidity += delta;
                    burningRate_W_Humidity = burningRate * std::exp(-humidity * k1);
                    ignitionRate_W_Humidity = ignitionRate * std::exp(-humidity * k2);
                    break;
                case GLFW_KEY_D:
                    diffusionRate += delta;
                    break;
                case GLFW_KEY_B:
                    burningRate += delta;
                    burningRate_W_Humidity = burningRate * std::exp(-humidity * k1);
                    break;
                case GLFW_KEY_I:
                    ignitionRate += delta;
                    ignitionRate_W_Humidity = ignitionRate * std::exp(-humidity * k2);
                    break;
                case GLFW_KEY_C:
                    coolingRate += delta;
                    break;
                case GLFW_KEY_F:
                    temperatureIncreaseRate += 10.0f;
                    break;
                case GLFW_KEY_2:
                    fireTerrain = CUBE;
                    voxelGridWidth = 5;
                    voxelGridHeight = 5;
                    voxelGridDepth = 5;
                    fireSimulator = new FireSimulator(voxelGridWidth, voxelGridHeight, voxelGridDepth, fireTerrain);
                    fireRenderer = new FireRenderer(*fireSimulator);
                    burning = true;
                    firstBurn = true;
                    allIgnited = false;
                    firstIgnite = true;
                    startTime = std::chrono::high_resolution_clock::now();
                    break;
                case GLFW_KEY_1:
                    fireTerrain = TEST;
                    voxelGridWidth = 10;
                    voxelGridHeight = 10;
                    voxelGridDepth = 10;
                    fireSimulator = new FireSimulator(voxelGridWidth, voxelGridHeight, voxelGridDepth, fireTerrain);
                    fireRenderer = new FireRenderer(*fireSimulator);
                    burning = true;
                    firstBurn = true;
                    allIgnited = false;
                    firstIgnite = true;
                    startTime = std::chrono::high_resolution_clock::now();
                    break;
                case GLFW_KEY_3:
                    fireTerrain = CUBE;
                    voxelGridWidth = 10;
                    voxelGridHeight = 1;
                    voxelGridDepth = 10;
                    fireSimulator = new FireSimulator(voxelGridWidth, voxelGridHeight, voxelGridDepth, fireTerrain);
                    fireRenderer = new FireRenderer(*fireSimulator);
                    burning = true;
                    firstBurn = true;
                    allIgnited = false;
                    firstIgnite = true;
                    startTime = std::chrono::high_resolution_clock::now();
                    break;
                case GLFW_KEY_4:
                    fireTerrain = PERLIN;
                    voxelGridWidth = 5;
                    voxelGridHeight = 5;
                    voxelGridDepth = 5;
                    fireSimulator = new FireSimulator(voxelGridWidth, voxelGridHeight, voxelGridDepth, fireTerrain);
                    fireRenderer = new FireRenderer(*fireSimulator);
                    burning = true;
                    firstBurn = true;
                    allIgnited = false;
                    firstIgnite = true;
                    startTime = std::chrono::high_resolution_clock::now();
                    break;
                case GLFW_KEY_5:
                    fireTerrain = PERLIN;
                    voxelGridWidth = 10;
                    voxelGridHeight = 10;
                    voxelGridDepth = 10;
                    fireSimulator = new FireSimulator(voxelGridWidth, voxelGridHeight, voxelGridDepth, fireTerrain);
                    fireRenderer = new FireRenderer(*fireSimulator);
                    burning = true;
                    firstBurn = true;
                    allIgnited = false;
                    firstIgnite = true;
                    startTime = std::chrono::high_resolution_clock::now();
                    break;
                case GLFW_KEY_6:
                    fireTerrain = PERLIN;
                    voxelGridWidth = 10;
                    voxelGridHeight = 1;
                    voxelGridDepth = 10;
                    fireSimulator = new FireSimulator(voxelGridWidth, voxelGridHeight, voxelGridDepth, fireTerrain);
                    burning = true;
                    firstBurn = true;
                    allIgnited = false;
                    firstIgnite = true;
                    startTime = std::chrono::high_resolution_clock::now();
                    break;
                case GLFW_KEY_7:
                    fireTerrain = SPHERE;
                    voxelGridWidth = 5;
                    voxelGridHeight = 5;
                    voxelGridDepth = 5;
                    fireSimulator = new FireSimulator(voxelGridWidth, voxelGridHeight, voxelGridDepth, fireTerrain);
                    fireRenderer = new FireRenderer(*fireSimulator);
                    burning = true;
                    firstBurn = true;
                    allIgnited = false;
                    firstIgnite = true;
                    startTime = std::chrono::high_resolution_clock::now();
                    break;
                case GLFW_KEY_8:
                    fireTerrain = SPHERE;
                    voxelGridWidth = 10;
                    voxelGridHeight = 10;
                    voxelGridDepth = 10;
                    fireSimulator = new FireSimulator(voxelGridWidth, voxelGridHeight, voxelGridDepth, fireTerrain);
                    fireRenderer = new FireRenderer(*fireSimulator);
                    burning = true;
                    firstBurn = true;
                    allIgnited = false;
                    firstIgnite = true;
                    startTime = std::chrono::high_resolution_clock::now();
                    break;
                case GLFW_KEY_9:
                    fireTerrain = SPHERE;
                    voxelGridWidth = 10;
                    voxelGridHeight = 1;
                    voxelGridDepth = 10;
                    fireSimulator = new FireSimulator(voxelGridWidth, voxelGridHeight, voxelGridDepth, fireTerrain);
                    fireRenderer = new FireRenderer(*fireSimulator);
                    burning = true;
                    firstBurn = true;
                    allIgnited = false;
                    firstIgnite = true;
                    startTime = std::chrono::high_resolution_clock::now();
                    break;
            }
        }
    }
}

void renderImGuiOverlay() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_Always);
    ImGui::Begin("Simulation Variables", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

    ImGui::Text("Wind Direction: (%.2f, %.2f, %.2f)", windDirection.x, windDirection.y, windDirection.z);
    ImGui::Text("Wind Intensity: %.2f", windIntensity);
    ImGui::Text("Humidity: %.2f", humidity);
    ImGui::Text("Diffusion Rate: %.2f", diffusionRate);
    ImGui::Text("Burning Rate: %.2f", burningRate);
    ImGui::Text("Burning Rate /W Humidity: %.2f", burningRate_W_Humidity);
    ImGui::Text("Ignition Rate: %.2f", ignitionRate);
    ImGui::Text("Ignition Rate /W Humidity: %.2f", ignitionRate_W_Humidity);
    ImGui::Text("Cooling Rate: %.2f", coolingRate);
    ImGui::Text("Temperature Increase Rate: %.2f", temperatureIncreaseRate);
    ImGui::Text("Total Ignite Time: %.2f", elapsedTimeIgnited);
    ImGui::Text("Total Burn Time: %.2f", elapsedTime);
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

int main() {
    // Initialize GLFW, create window, and load OpenGL functions

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Set OpenGL version to 3.3 core profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1600, 1600, "Fire Simulation", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, nullptr);
    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Load OpenGL functions using GLEW
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // Set up view and projection matrices for the camera
    glm::mat4 view = glm::lookAt(glm::vec3(-20.0f, 30.0f, -20.0f),
                                  glm::vec3(0.0f, 0.0f, 0.0f),
                                  glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);


    // Default simulation variables values
    windDirection = glm::vec3(0.0f, 0.0f, 0.0f); 
    windIntensity = 0.5f; 
    humidity = 0.3f; 
    diffusionRate = 0.2f; 
    burningRate = 0.1f; 
    k1 = 0.5f;
    burningRate_W_Humidity = burningRate * std::exp(-humidity * k1);
    ignitionRate = 0.4f; 
    k2 = 0.5f;
    ignitionRate_W_Humidity = ignitionRate * std::exp(-humidity * k2);
    coolingRate = 200.0f; 
    fireTerrain = TEST;
    temperatureIncreaseRate = 40.0f;

    voxelGridWidth = 10;
    voxelGridHeight = 10;
    voxelGridDepth = 10;

    fireSimulator = new FireSimulator(voxelGridWidth, voxelGridHeight, voxelGridDepth, fireTerrain);
    fireRenderer = new FireRenderer(*fireSimulator);
    std::cout << "Shaders compiled and linked successfully" << std::endl;

    // Timing variables
    startTime = std::chrono::high_resolution_clock::now();


    float deltaTime = 0.02f;
    burning = true;
    firstBurn = true;
    allIgnited = false;
    firstIgnite = true;

    std::pair<bool, bool> updateResult;

    // Main render loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Clear the screen
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // currentTime = std::chrono::high_resolution_clock::now();
        // float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        // lastTime = currentTime;

        view = glm::lookAt(cameraPos, glm::vec3(0.0f, 5.0f, 0.0f), cameraUp);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        glDepthMask(GL_FALSE);


        if (!simulationPaused) {
            updateResult = fireSimulator->update(deltaTime, windDirection, windIntensity, humidity, diffusionRate, burningRate_W_Humidity, ignitionRate_W_Humidity, coolingRate, temperatureIncreaseRate);
            burning = updateResult.first;
            allIgnited = updateResult.second;
            midTime = std::chrono::high_resolution_clock::now();
            if (!burning) {
                // stop burning clock
                if (firstBurn) {
                    endTimeBurn = std::chrono::high_resolution_clock::now();
                    elapsedTime = std::chrono::duration<float>(endTimeBurn - startTime).count();
                    firstBurn = false;
                }
            } else {
                elapsedTime = std::chrono::duration<float>(midTime - startTime).count();
            }
            if (allIgnited) {
                // stop ignition clock
                if (firstIgnite) {
                    endTimeIgnite = std::chrono::high_resolution_clock::now();
                    elapsedTimeIgnited = std::chrono::duration<float>(endTimeIgnite - startTime).count();
                    firstIgnite = false;
                }
            } else {
                elapsedTimeIgnited = std::chrono::duration<float>(midTime - startTime).count();
            }
        }

        fireRenderer->render(view, projection);

        glDepthMask(GL_TRUE);

        renderImGuiOverlay();

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    delete fireRenderer;
    delete fireSimulator;

    glfwTerminate();
    return 0;
}