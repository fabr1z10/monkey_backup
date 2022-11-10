#include "random.h"

#include <memory>

Random::Random() {
    std::random_device rd;
    m_gen = std::make_unique<std::mt19937>(rd());
}

int Random::getUniform(int min, int max) {
    std::uniform_int_distribution<> dis(min, max);
    return dis(*(m_gen));

}

float Random::getUniformReal (float min, float max) {
    std::uniform_real_distribution<> dis(min, max);
    return dis(*(m_gen));
}