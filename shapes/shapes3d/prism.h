#pragma once


#include "../../shape.h"
#include <pybind11/pybind11.h>

// default is blocking
struct Edge {
    Edge(glm::vec2 A, glm::vec2 B);
    glm::vec2 A;
    glm::vec2 B;
    int mode;   // 0 = blocking, 1 = non-blocking
    float y0;   // default value inf
};

class Prism : public Shape {
public:
    Prism(const pybind11::kwargs&);
    const std::vector<glm::vec2>& getPoints() const;
    const std::vector<Edge>& getEdges() const;
private:
    std::vector<glm::vec2> m_points;
    std::vector<Edge> m_edges;

};

inline const std::vector<glm::vec2>& Prism::getPoints() const {
    return m_points;
}

inline const std::vector<Edge>& Prism::getEdges() const {
    return m_edges;
}


