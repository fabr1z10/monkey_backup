#pragma once

#include "../shape.h"
#include <vector>
#include <array>


struct Tri {
    // points
    std::array<glm::vec2, 3> points;
    // normal (axes)
    std::array<glm::vec2, 3> axes;
    // we store them here as they don't change except fro translation
    std::array<glm::vec2, 3> ownProj;
};

class Triangles : public Shape2D {
public:
    Triangles(const std::vector<glm::vec2>& boundary);
    void addTriangle(glm::vec2 A, glm::vec2 B, glm::vec2 C);
    //void addBoundaryPoint(glm::vec2 A);
    //const std::vector<glm::vec2>& axes() const;
    glm::vec2 project(glm::vec2, const glm::mat4&) const override;
    const std::vector<Tri>& getTri() const;
private:

    //std::vector<glm::vec2> m_axes;
    //std::vector<glm::vec2> m_points;
    std::vector<Tri> m_triangles;
};

//inline const std::vector<glm::vec2> & Triangles::axes() const {
//    return m_axes;
//}


inline const std::vector<Tri> & Triangles::getTri() const {
    return m_triangles;
}