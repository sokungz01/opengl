#ifndef ASSIGNMENT3_GROUND_H
#define ASSIGNMENT3_GROUND_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <learnopengl/shader_m.h>

class Ground {
public:
    Ground();
    void init(const std::string &texturePath);
    void draw(Shader &shader);
    unsigned int getTexture() const { return texture; }
private:
    unsigned int VAO, VBO;
    // wall geometry for the map border
    unsigned int wallVAO, wallVBO;
    unsigned int texture;
};

#endif
