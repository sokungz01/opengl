#include "Coins.h"
#include <glad/glad.h>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

static const int COIN_SEGMENTS = 32;
static const float BASE_RADIUS = 0.5f;
static const float BASE_HALF_HEIGHT = 0.05f;
static const float COIN_SCALE = 1.75f; // make bigger by increasing this

Coins::Coins()
    : VAO(0), VBO(0)
{
}

void Coins::init()
{
    const int segments = COIN_SEGMENTS;
    const float baseRadius = BASE_RADIUS;
    const float halfHeight = BASE_HALF_HEIGHT; // total height = 2*halfHeight
    std::vector<float> verts;
    verts.reserve(segments * 8 * 8);

    for (int i = 0; i < segments; ++i) {
        int next = (i + 1) % segments;
        // center top
        verts.push_back(0.0f); verts.push_back(halfHeight); verts.push_back(0.0f);
        verts.push_back(0.0f); verts.push_back(1.0f); verts.push_back(0.0f);
        verts.push_back(0.5f); verts.push_back(0.5f);

        // current edge
    float ang = (float)i / (float)segments * 2.0f * (float)M_PI;
        float x = cosf(ang) * baseRadius;
        float z = sinf(ang) * baseRadius;
        verts.push_back(x); verts.push_back(halfHeight); verts.push_back(z);
        verts.push_back(0.0f); verts.push_back(1.0f); verts.push_back(0.0f);
        verts.push_back((x / baseRadius) * 0.5f + 0.5f); verts.push_back((z / baseRadius) * 0.5f + 0.5f);

        // next edge
    float ang2 = (float)next / (float)segments * 2.0f * (float)M_PI;
        float x2 = cosf(ang2) * baseRadius;
        float z2 = sinf(ang2) * baseRadius;
        verts.push_back(x2); verts.push_back(halfHeight); verts.push_back(z2);
        verts.push_back(0.0f); verts.push_back(1.0f); verts.push_back(0.0f);
        verts.push_back((x2 / baseRadius) * 0.5f + 0.5f); verts.push_back((z2 / baseRadius) * 0.5f + 0.5f);
    }

    // bottom cap
    for (int i = 0; i < segments; ++i) {
        int next = (i + 1) % segments;
        // center bottom
        verts.push_back(0.0f); verts.push_back(-halfHeight); verts.push_back(0.0f);
        verts.push_back(0.0f); verts.push_back(-1.0f); verts.push_back(0.0f);
        verts.push_back(0.5f); verts.push_back(0.5f);

        // next edge (reversed winding)
    float ang2 = (float)next / (float)segments * 2.0f * (float)M_PI;
    float x2 = cosf(ang2) * baseRadius;
    float z2 = sinf(ang2) * baseRadius;
        verts.push_back(x2); verts.push_back(-halfHeight); verts.push_back(z2);
        verts.push_back(0.0f); verts.push_back(-1.0f); verts.push_back(0.0f);
        verts.push_back((x2 / baseRadius) * 0.5f + 0.5f); verts.push_back((z2 / baseRadius) * 0.5f + 0.5f);

        // current edge
        float ang = (float)i / (float)segments * 2.0f * M_PI;
        float x = cosf(ang) * baseRadius;
        float z = sinf(ang) * baseRadius;
        verts.push_back(x); verts.push_back(-halfHeight); verts.push_back(z);
        verts.push_back(0.0f); verts.push_back(-1.0f); verts.push_back(0.0f);
        verts.push_back((x / baseRadius) * 0.5f + 0.5f); verts.push_back((z / baseRadius) * 0.5f + 0.5f);
    }

    // side (two triangles per segment)
    for (int i = 0; i < segments; ++i) {
        int next = (i + 1) % segments;
        float ang = (float)i / (float)segments * 2.0f * (float)M_PI;
        float ang2 = (float)next / (float)segments * 2.0f * (float)M_PI;
        float x = cosf(ang) * baseRadius;
        float z = sinf(ang) * baseRadius;
        float x2 = cosf(ang2) * baseRadius;
        float z2 = sinf(ang2) * baseRadius;
        // compute normal (approx)
        float nx = cosf((ang + ang2) * 0.5f);
        float nz = sinf((ang + ang2) * 0.5f);

        // first triangle: top current, bottom current, top next
        verts.push_back(x); verts.push_back(halfHeight); verts.push_back(z);
        verts.push_back(x); verts.push_back(0.0f); verts.push_back(z);
        verts.push_back((float)i / segments); verts.push_back(0.0f);

        verts.push_back(x); verts.push_back(-halfHeight); verts.push_back(z);
        verts.push_back(x); verts.push_back(0.0f); verts.push_back(z);
        verts.push_back((float)i / segments); verts.push_back(1.0f);

        verts.push_back(x2); verts.push_back(halfHeight); verts.push_back(z2);
        verts.push_back(x2); verts.push_back(0.0f); verts.push_back(z2);
        verts.push_back((float)(i + 1) / segments); verts.push_back(0.0f);

        // second triangle: top next, bottom current, bottom next
        verts.push_back(x2); verts.push_back(halfHeight); verts.push_back(z2);
        verts.push_back(x2); verts.push_back(0.0f); verts.push_back(z2);
        verts.push_back((float)(i + 1) / segments); verts.push_back(0.0f);

        verts.push_back(x); verts.push_back(-halfHeight); verts.push_back(z);
        verts.push_back(x); verts.push_back(0.0f); verts.push_back(z);
        verts.push_back((float)i / segments); verts.push_back(1.0f);

        verts.push_back(x2); verts.push_back(-halfHeight); verts.push_back(z2);
        verts.push_back(x2); verts.push_back(0.0f); verts.push_back(z2);
        verts.push_back((float)(i + 1) / segments); verts.push_back(1.0f);
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_STATIC_DRAW);
    glBindVertexArray(VAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    vertexCount = static_cast<int>(verts.size() / 8);

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
    glBindVertexArray(VAO);
    for (size_t i = 0; i < positions.size(); ++i) {
        if (collected[i]) continue;
        glm::mat4 m(1.0f);
        float t = static_cast<float>(glfwGetTime());

        // per-coin bobbing (vertical)
        float amp = 0.08f;
        float freq = 3.0f;
        float phase = 0.0f;
        if (i < bobAmplitude.size()) amp = bobAmplitude[i];
        if (i < bobFrequency.size()) freq = bobFrequency[i];
        if (i < bobPhase.size()) freq = bobFrequency[i], phase = bobPhase[i];

        float bounce = sinf(t * freq + phase) * amp;
        float bounceAbs = fabsf(bounce);
        float baseLift = BASE_HALF_HEIGHT * COIN_SCALE;
        m = glm::translate(m, positions[i] + glm::vec3(0.0f, baseLift + bounceAbs, 0.0f));

        // flip 90 degrees so coin face is vertical, then spin around local axis
        m = glm::rotate(m, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        float spin = t * 180.0f; // degrees per second
        m = glm::rotate(m, glm::radians(spin), glm::vec3(0.0f, 0.0f, 1.0f));
        m = glm::scale(m, glm::vec3(COIN_SCALE));

        // color by value: rare coins a bit more orange
        shader.setBool("useColor", true);
        if (i < value.size() && value[i] == 2) {
            shader.setVec3("objectColor", glm::vec3(1.0f, 0.85f, 0.0f));
        } else {
            shader.setVec3("objectColor", glm::vec3(1.0f, 1.0f, 0.0f));
        }

        shader.setMat4("model", m);
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    }
    shader.setBool("useColor", false);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
