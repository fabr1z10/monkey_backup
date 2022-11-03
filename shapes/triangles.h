#pragma once

#include "../shape.h"
#include <vector>


class Triangles : public Shape2D {
public:
    Triangles(const std::vector<glm::vec2>& boundary);
    void addTriangle(glm::vec2 A, glm::vec2 B, glm::vec2 C);
    void addBoundaryPoint(glm::vec2 A);
    const std::vector<glm::vec2>& axes() const;
    glm::vec2 project(glm::vec2, const glm::mat4&) const override;

private:
    std::vector<glm::vec2> m_axes;
    std::vector<glm::vec2> m_points;
};

inline const std::vector<glm::vec2> & Triangles::axes() const {
    return m_axes;
}
