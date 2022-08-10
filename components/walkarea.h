#pragma once

#include <pybind11/pybind11.h>
#include "../component.h"
#include <glm/glm.hpp>
#include "../graph.h"

struct EdgeData {
    glm::vec2 unitVec;
    glm::vec2 start, end;
    glm::vec2 normalInAtStart;
    glm::vec2 normalInAtEnd;
    glm::vec2 normal;
    float length;
    bool isHole;
};




class WalkArea : public Component {
public:
    WalkArea(const pybind11::kwargs& );
    void start() override;
    bool isPointInside (glm::vec2);
    glm::vec2 closestPointIn(glm::vec2);
    std::vector<glm::vec2> getPath(glm::vec2 start, glm::vec2 target);
private:
    void addPath(const std::vector<glm::vec2>&, bool isHole, int);
    bool checkEdges(const std::vector<glm::vec2>&, glm::vec2 A, glm::vec2 B);
    int addNode(glm::vec2);
    std::vector<glm::vec2> m_outline;
    std::vector<std::vector<glm::vec2>> m_holes;
    int m_nodeCount;

    std::shared_ptr<Graph<glm::vec2>> m_graph;
    std::vector<EdgeData> m_edgeData;
};
