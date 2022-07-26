#pragma once

// this special class is just a box of fixed size that cannot rotate.
// this is ok for handling collisions in platformer or other games where
// the nodes can only translate but never rotate!

#include "../shape.h"
#include <array>

class AABB : public Shape2D {
public:
    AABB(float xmin, float xmax, float ymin, float ymax);
    glm::vec2 project(glm::vec2, const glm::mat4&) const override;
private:
    float m_xMin;
    float m_xMax;
    float m_yMin;
    float m_yMax;
    std::array<glm::vec2, 4> m_points;
};