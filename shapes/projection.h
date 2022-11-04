#pragma once

#include <glm/glm.hpp>


struct Projection {
    Projection() : m(std::numeric_limits<float>::infinity()), M(-std::numeric_limits<float>::infinity()) {}
    Projection(glm::vec2 value) : m(value.x), M(value.y) {}
    void update(float x) {
        m = std::min(x, m);
        M = std::max(x, M);
    }
    bool intersect(const Projection& other) const {
        return ! (other.m > M || m > other.M);
    }
    float m;
    float M;

};
