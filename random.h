#pragma once

#include <random>
#include <memory>

class Random {
private:
    Random();
public:
    static Random& instance() {
        static Random instance; // Guaranteed to be destroyed.
        return instance;
    }
    int getUniform (int min, int max);
    float getUniformReal (float min, float max);
private:
    std::unique_ptr<std::mt19937> m_gen;
};