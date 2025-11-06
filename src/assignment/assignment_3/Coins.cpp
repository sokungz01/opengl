#include "Coins.h"
#include <glad/glad.h>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <learnopengl/model.h>
#include <learnopengl/filesystem.h>

static const int COIN_SEGMENTS = 32;
static const float BASE_RADIUS = 0.5f;
static const float BASE_HALF_HEIGHT = 0.05f;
static const float COIN_SCALE = 0.75f;

Coins::Coins()
    : coinModel(nullptr)
{
}
void Coins::init()
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}


void Coins::spawnRandom(int count)
{
    positions.clear();
    collected.clear();
    // per-coin gameplay attributes
    value.clear();
    bobAmplitude.clear();
    bobFrequency.clear();
    bobPhase.clear();

    const float desiredFaceHeight = 1.0f; // world height of coin face center
    const float baseLift = BASE_HALF_HEIGHT * COIN_SCALE;
    const float storedY = desiredFaceHeight - baseLift;

    for (int i = 0; i < count; ++i) {
        float x = (std::rand() / (float)RAND_MAX) * 60.0f - 20.0f;
        float z = (std::rand() / (float)RAND_MAX) * 60.0f - 20.0f;
        positions.push_back(glm::vec3(x, storedY, z));
        collected.push_back(false);

        // 20% chance for a rare double-value coin
        int v = (std::rand() % 100) < 20 ? 2 : 1;
        value.push_back(v);

        // bobbing parameters
        float amp = 0.04f + (std::rand() / (float)RAND_MAX) * 0.08f; // 0.04 - 0.12
        float freq = 2.0f + (std::rand() / (float)RAND_MAX) * 3.0f;   // 2 - 5
        float phase = (std::rand() / (float)RAND_MAX) * 2.0f * (float)M_PI;
        bobAmplitude.push_back(amp);
        bobFrequency.push_back(freq);
        bobPhase.push_back(phase);
    }
}

int Coins::updateCollect(const glm::vec3 &carPos, float carRadius)
{
    int newly = 0;
    for (size_t i = 0; i < positions.size(); ++i) {
        if (collected[i]) continue;
        // coin base radius is BASE_RADIUS, scaled by COIN_SCALE
        const float coinRadius = COIN_SCALE * BASE_RADIUS;
        float dist = glm::distance(glm::vec2(carPos.x, carPos.z), glm::vec2(positions[i].x, positions[i].z));
        float thresh = carRadius + coinRadius + 0.05f; // small margin
        if (dist < thresh) {
            collected[i] = true;
            // add coin's value (1 or 2)
            if (i < value.size()) newly += value[i];
            else newly++;
        }
    }
    return newly;
}

int Coins::remaining() const
{
    int rem = 0;
    for (size_t i = 0; i < collected.size(); ++i) if (!collected[i]) ++rem;
    return rem;
}

void Coins::draw(Shader &shader, unsigned int fallbackTexture)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fallbackTexture);
    shader.setInt("texture_diffuse1", 0);
    for (size_t i = 0; i < positions.size(); ++i) {
        if (collected[i]) continue;
        glm::mat4 m(1.0f);
        float t = static_cast<float>(glfwGetTime());

        float amp = 0.08f;
        float freq = 3.0f;
        float phase = 0.0f;
        if (i < bobAmplitude.size()) amp = bobAmplitude[i];
        if (i < bobFrequency.size()) freq = bobFrequency[i];
        if (i < bobPhase.size()) phase = bobPhase[i];

        float bounce = sinf(t * freq + phase) * amp;
        float bounceAbs = fabsf(bounce);
        float baseLift = BASE_HALF_HEIGHT * COIN_SCALE;
        m = glm::translate(m, positions[i] + glm::vec3(0.0f, baseLift + bounceAbs, 0.0f));

        m = glm::rotate(m, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        float spin = t * 180.0f;
        m = glm::rotate(m, glm::radians(spin), glm::vec3(0.0f, 1.0f, 0.0f));
        m = glm::scale(m, glm::vec3(COIN_SCALE));

        shader.setBool("useColor", true);
        if (i < value.size() && value[i] == 2) {
            shader.setVec3("objectColor", glm::vec3(1.0f, 0.85f, 0.0f));
        } else {
            shader.setVec3("objectColor", glm::vec3(1.0f, 1.0f, 0.0f));
        }

        shader.setMat4("model", m);
        if (coinModel) {
            coinModel->Draw(shader);
        }
    }
    shader.setBool("useColor", false);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
