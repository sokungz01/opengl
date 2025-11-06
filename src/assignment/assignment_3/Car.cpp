#include "Car.h"
#include <glm/gtc/type_ptr.hpp>
#include <cfloat>

Car::Car()
        : Position(0.0f, 0.0f, 0.0f), Yaw(0.0f), Front(1.0f,0.0f, 0.0f),
            Velocity(0.0f), MaxSpeed(24.0f), Accel(16.0f), Brake(18.0f), Friction(3.5f),
            ModelCenter(0.0f), ModelForwardLocal(0.0f,0.0f,0.0f), PitchCorrection(0.0f), ModelRadius(1.0f)
{
}

void Car::init(const Model &model)
{
    // compute AABB center
    bool any = false;
    glm::vec3 minp(FLT_MAX), maxp(-FLT_MAX);
    for (auto &mesh : model.meshes) {
        for (auto &v : mesh.vertices) {
            glm::vec3 p = v.Position;
            minp = glm::min(minp, p);
            maxp = glm::max(maxp, p);
            any = true;
        }
    }
    if (any) ModelCenter = (minp + maxp) * 0.5f;

    // compute model-space radius (max distance from center to vertex)
    {
        float maxd = 0.0f;
        for (auto &mesh : model.meshes) {
            for (auto &v : mesh.vertices) {
                float d = glm::length(v.Position - ModelCenter);
                if (d > maxd) maxd = d;
            }
        }
        if (maxd > 0.0f) ModelRadius = maxd;
    }

    {
        glm::mat3 cov(0.0f);
        size_t count = 0;
        for (auto &mesh : model.meshes) {
            for (auto &v : mesh.vertices) {
                glm::vec3 d = v.Position - ModelCenter;
                cov[0][0] += d.x * d.x; cov[0][1] += d.x * d.y; cov[0][2] += d.x * d.z;
                cov[1][0] += d.y * d.x; cov[1][1] += d.y * d.y; cov[1][2] += d.y * d.z;
                cov[2][0] += d.z * d.x; cov[2][1] += d.z * d.y; cov[2][2] += d.z * d.z;
                ++count;
            }
        }
        if (count > 0) {
            cov /= (float)count;
            glm::vec3 axis(1.0f, 0.0f, 0.0f);
            for (int it = 0; it < 20; ++it) {
                glm::vec3 next;
                next.x = cov[0][0]*axis.x + cov[0][1]*axis.y + cov[0][2]*axis.z;
                next.y = cov[1][0]*axis.x + cov[1][1]*axis.y + cov[1][2]*axis.z;
                next.z = cov[2][0]*axis.x + cov[2][1]*axis.y + cov[2][2]*axis.z;
                float len = glm::length(next);
                if (len < 1e-6f) break;
                axis = next / len;
            }
            axis.y = 0.0f;
            if (glm::length(axis) > 1e-3f) ModelForwardLocal = glm::normalize(axis);
        }
    }
}

void Car::handleInput(GLFWwindow* window, float deltaTime)
{
    bool pressW = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
    bool pressS = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
    bool pressA = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
    bool pressD = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;

    if (pressW) {
        Velocity += Accel * deltaTime;
    } else if (pressS) {
        if (Velocity > 0.0f)
            Velocity -= Brake * deltaTime;
        else
            Velocity -= (Brake * 0.5f) * deltaTime;
    } else {
        if (Velocity > 0.0f) {
            Velocity -= Friction * deltaTime;
            if (Velocity < 0.0f) Velocity = 0.0f;
        } else if (Velocity < 0.0f) {
            Velocity += Friction * deltaTime;
            if (Velocity > 0.0f) Velocity = 0.0f;
        }
    }

    float steer = 0.0f;
    if (pressA) steer += 1.0f;
    if (pressD) steer -= 1.0f;
    float speedFactor = 0.0f;
    if (fabs(Velocity) > 0.01f) speedFactor = fabs(Velocity) / MaxSpeed;
    speedFactor = glm::clamp(speedFactor, 0.05f, 1.0f);
    Yaw += steer * 90.0f * speedFactor * deltaTime;
}

void Car::update(float deltaTime)
{
    if (Velocity > MaxSpeed) Velocity = MaxSpeed;
    if (Velocity < -MaxSpeed*0.5f) Velocity = -MaxSpeed*0.5f;
    glm::vec3 moveFront = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(Yaw), glm::vec3(0.0f,1.0f,0.0f)) * glm::vec4(ModelForwardLocal, 0.0f));
    moveFront.y = 0.0f;
    if (glm::length(moveFront) < 1e-6f) moveFront = glm::vec3(0.0f,0.0f,-1.0f);
    moveFront = glm::normalize(moveFront);
    Position += -moveFront * Velocity * deltaTime;
    Position.y = 0.5f;

    Front = moveFront;
}

glm::mat4 Car::getModelMatrix() const
{
    glm::mat4 m(1.0f);
    m = glm::translate(m, Position);
    m = glm::translate(m, ModelCenter);
    m = glm::rotate(m, glm::radians(Yaw), glm::vec3(0.0f,1.0f,0.0f));
    m = glm::rotate(m, glm::radians(PitchCorrection), glm::vec3(1.0f,0.0f,0.0f));
    m = glm::translate(m, -ModelCenter);
    m = glm::scale(m, glm::vec3(Scale));
    return m;
}

void Car::clampPosition(float minX, float maxX, float minZ, float maxZ)
{
    if (Position.x < minX) Position.x = minX;
    if (Position.x > maxX) Position.x = maxX;
    if (Position.z < minZ) Position.z = minZ;
    if (Position.z > maxZ) Position.z = maxZ;
}

float Car::getBoundingRadius() const
{
    return ModelRadius * Scale;
}
