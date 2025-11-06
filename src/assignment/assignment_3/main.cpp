#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include "Car.h"
#include "Ground.h"
#include "Coins.h"
#include "Utils.h"

#include <iostream>
#include <string>

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// match the ground plane size in Ground.cpp (half-extent)
const float PLANE_SIZE = 50.0f;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool cameraAttached = true;

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
int randomCoins = 50;

void setBackgroundColor() {
    glClearColor(0.80f, 0.90f, 1.00f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    if (!cameraAttached)
        camera.ProcessMouseMovement(xoffset, yoffset);
}

void processInput(GLFWwindow *window, Car &car) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    car.handleInput(window, deltaTime);
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Assignment 3 - Simple 3D Game", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    Shader ourShader("shader.vs", "shader.fs");
    Model carModel(FileSystem::getPath("resources/objects/f1/f1.obj"));

    Car car;
    car.init(carModel);

    Ground ground;
    ground.init(FileSystem::getPath("resources/textures/smooth-stone.png"));

    Coins coins;
    coins.init();
    coins.spawnRandom(randomCoins);

    unsigned int whiteTexture = utils::createWhiteTexture();

    int score = 0;

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window, car);

        enum CameraSide { SIDE_REAR = 0, SIDE_LEFT = 1, SIDE_RIGHT = 2 };
        static CameraSide cameraSide = SIDE_REAR;

        static int lastCState = GLFW_RELEASE;
        int cState = glfwGetKey(window, GLFW_KEY_C);
        if (cState == GLFW_PRESS && lastCState == GLFW_RELEASE) {
            cameraSide = static_cast<CameraSide>((cameraSide + 1) % 3);
        }
        lastCState = cState;

        static int lastBState = GLFW_RELEASE;
        int bState = glfwGetKey(window, GLFW_KEY_B);
        if (bState == GLFW_PRESS && lastBState == GLFW_RELEASE) {
            cameraAttached = !cameraAttached;
        }
        lastBState = bState;

        bool lookBack = glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS; // held to look forward from camera

        setBackgroundColor();

        car.update(deltaTime);

        car.clampPosition(-PLANE_SIZE + 0.5f, PLANE_SIZE - 0.5f, -PLANE_SIZE + 0.5f, PLANE_SIZE - 0.5f);

    int newly = coins.updateCollect(car.getPosition(), car.getBoundingRadius());
        if (newly > 0) {
            score += newly;
            std::string title = "Assignment 3 - Score: " + std::to_string(score);
            glfwSetWindowTitle(window, title.c_str());
            std::cout << "Collected " << newly << " coin(s). Score=" << score << "\n";
        }

        if (coins.remaining() == 0) {
            coins.spawnRandom(randomCoins);
            std::cout << "All coins collected! Respawning...\n";
        }

        ourShader.use();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);
        ourShader.setBool("useColor", false);

        ground.draw(ourShader);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, whiteTexture);
        ourShader.setInt("texture_diffuse1", 0);
        ourShader.setMat4("model", car.getModelMatrix());
        carModel.Draw(ourShader);
        glBindTexture(GL_TEXTURE_2D, 0);

        coins.draw(ourShader, whiteTexture);

        if (cameraAttached) {
            glm::vec3 carFront = car.getFront();
            glm::vec3 worldUp(0.0f, 1.0f, 0.0f);
            glm::vec3 carRight = glm::normalize(glm::cross(carFront, worldUp));

            float rearDist = 6.0f;
            float sideDist = 4.0f;

            glm::vec3 camOffset;
            if (cameraSide == SIDE_REAR) {
                camOffset = carFront * rearDist + glm::vec3(0.0f, 2.0f, 0.0f);
                if (lookBack) {
                    camOffset = -carFront * rearDist + glm::vec3(0.0f, 2.0f, 0.0f);
                }
            } else if (cameraSide == SIDE_LEFT) {
                camOffset = -carRight * sideDist + glm::vec3(0.0f, 1.6f, 0.0f);
            } else {
                camOffset = carRight * sideDist + glm::vec3(0.0f, 1.6f, 0.0f);
            }

            glm::vec3 desiredCamPos = car.getPosition() + camOffset;
            const float smoothTime = 0.09f;
            float alpha = 0.0f;
            if (deltaTime > 0.0f) alpha = 1.0f - expf(-deltaTime / smoothTime);
            camera.Position = glm::mix(camera.Position, desiredCamPos, alpha);
            glm::vec3 desiredFront;
            if (lookBack) {
                desiredFront = glm::normalize(carFront);
            } else {
                desiredFront = glm::normalize((car.getPosition() + glm::vec3(0.0f,1.0f,0.0f)) - desiredCamPos);
            }
            camera.Front = glm::normalize(glm::mix(camera.Front, desiredFront, alpha));
            camera.Right = glm::normalize(glm::cross(camera.Front, camera.WorldUp));
            camera.Up    = glm::normalize(glm::cross(camera.Right, camera.Front));
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
