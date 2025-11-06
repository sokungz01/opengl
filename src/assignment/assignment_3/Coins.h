#ifndef ASSIGNMENT3_COINS_H
#define ASSIGNMENT3_COINS_H

#include <vector>
#include <glm/glm.hpp>
#include <learnopengl/shader_m.h>
#include <learnopengl/model.h>
#include <memory>

class Coins {
public:
    Coins();
    void init();
    void spawnRandom(int count);
    int updateCollect(const glm::vec3 &carPos, float carRadius);
    void draw(Shader &shader, unsigned int fallbackTexture);
    int remaining() const;
    void setModel(Model *m) { coinModel = m; }
    
private:
    Model *coinModel = nullptr;

    std::vector<glm::vec3> positions;
    std::vector<bool> collected;
    std::vector<int> value;
    std::vector<float> bobAmplitude;
    std::vector<float> bobFrequency;
    std::vector<float> bobPhase;    
};

#endif
