#ifndef ASSIGNMENT3_CAR_H
#define ASSIGNMENT3_CAR_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <learnopengl/model.h>
#include <GLFW/glfw3.h>

class Car {
public:
    Car();
    void init(const Model &model);
    void handleInput(GLFWwindow* window, float deltaTime);
    void update(float deltaTime);
    glm::mat4 getModelMatrix() const;
    glm::vec3 getPosition() const { return Position; }
    glm::vec3 getFront() const { return Front; }
    float Scale = 0.01f;

    void clampPosition(float minX, float maxX, float minZ, float maxZ);
    float getBoundingRadius() const;

private:
    glm::vec3 Position;
    float Yaw;
    glm::vec3 Front;

    float Velocity;
    float MaxSpeed;
    float Accel;
    float Brake;
    float Friction;

    glm::vec3 ModelCenter;
    glm::vec3 ModelForwardLocal;
    float PitchCorrection;
    float ModelRadius;
};

#endif
