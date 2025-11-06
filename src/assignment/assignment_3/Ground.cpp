#include "Ground.h"
#include "Utils.h"
#include <glad/glad.h>
#include <learnopengl/filesystem.h>

Ground::Ground()
    : VAO(0), VBO(0), texture(0)
{
}

void Ground::init(const std::string &texturePath)
{
    float planeSize = 50.0f;
    float tileCount = 50.0f;
    float planeVertices[] = {
        -planeSize, 0.0f, -planeSize,           0.0f, 1.0f, 0.0f,  0.0f, tileCount,
         planeSize, 0.0f, -planeSize,           0.0f, 1.0f, 0.0f,  tileCount, tileCount,
         planeSize, 0.0f,  planeSize,           0.0f, 1.0f, 0.0f,  tileCount, 0.0f,

        -planeSize, 0.0f, -planeSize,           0.0f, 1.0f, 0.0f,  0.0f, tileCount,
         planeSize, 0.0f,  planeSize,           0.0f, 1.0f, 0.0f,  tileCount, 0.0f,
        -planeSize, 0.0f,  planeSize,           0.0f, 1.0f, 0.0f,  0.0f, 0.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    texture = utils::loadTexture(texturePath.c_str());

    // build a unit cube (pos, normal, tex) for wall rendering
    float cubeVertices[] = {
        // positions          // normals           // texcoords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f, 1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
         0.5f, -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
         0.5f,  0.5f, -0.5f, 1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
         0.5f, -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
         0.5f,  0.5f,  0.5f, 1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
         0.5f, -0.5f,  0.5f, 1.0f,  0.0f,  0.0f,   0.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f,  0.0f,   0.0f, 1.0f,
         0.5f, -0.5f, -0.5f, 0.0f, -1.0f,  0.0f,   1.0f, 1.0f,
         0.5f, -0.5f,  0.5f, 0.0f, -1.0f,  0.0f,   1.0f, 0.0f,
         0.5f, -0.5f,  0.5f, 0.0f, -1.0f,  0.0f,   1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, -1.0f,  0.0f,   0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f,  0.0f,   0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f,  0.0f,    0.0f, 1.0f,
         0.5f,  0.5f,  0.5f, 0.0f, 1.0f,  0.0f,    1.0f, 0.0f,
         0.5f,  0.5f, -0.5f, 0.0f, 1.0f,  0.0f,    1.0f, 1.0f,
         0.5f,  0.5f,  0.5f, 0.0f, 1.0f,  0.0f,    1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f,  0.0f,    0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f, 0.0f, 1.0f,  0.0f,    0.0f, 0.0f
    };

    glGenVertexArrays(1, &wallVAO);
    glGenBuffers(1, &wallVBO);
    glBindVertexArray(wallVAO);
    glBindBuffer(GL_ARRAY_BUFFER, wallVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
}

void Ground::draw(Shader &shader)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    shader.setInt("texture_diffuse1", 0);
    glm::mat4 groundModel = glm::mat4(1.0f);
    shader.setMat4("model", groundModel);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // draw border walls around the plane
    glBindVertexArray(wallVAO);
    // wall parameters
    float planeSize = 50.0f;
    float wallHeight = 6.0f;
    float wallThickness = 1.0f;

    // front/back walls (along X, at Z = +/- planeSize)
    glm::mat4 m1 = glm::mat4(1.0f);
    m1 = glm::translate(m1, glm::vec3(0.0f, wallHeight * 0.5f, -planeSize - wallThickness * 0.5f));
    m1 = glm::scale(m1, glm::vec3(planeSize * 2.0f, wallHeight, wallThickness));
    shader.setMat4("model", m1);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glm::mat4 m2 = glm::mat4(1.0f);
    m2 = glm::translate(m2, glm::vec3(0.0f, wallHeight * 0.5f, planeSize + wallThickness * 0.5f));
    m2 = glm::scale(m2, glm::vec3(planeSize * 2.0f, wallHeight, wallThickness));
    shader.setMat4("model", m2);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // left/right walls (along Z, at X = +/- planeSize)
    glm::mat4 m3 = glm::mat4(1.0f);
    m3 = glm::translate(m3, glm::vec3(-planeSize - wallThickness * 0.5f, wallHeight * 0.5f, 0.0f));
    m3 = glm::scale(m3, glm::vec3(wallThickness, wallHeight, planeSize * 2.0f));
    shader.setMat4("model", m3);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glm::mat4 m4 = glm::mat4(1.0f);
    m4 = glm::translate(m4, glm::vec3(planeSize + wallThickness * 0.5f, wallHeight * 0.5f, 0.0f));
    m4 = glm::scale(m4, glm::vec3(wallThickness, wallHeight, planeSize * 2.0f));
    shader.setMat4("model", m4);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glBindVertexArray(0);
}
