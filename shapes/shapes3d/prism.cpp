#include "prism.h"


Edge::Edge(glm::vec2 A, glm::vec2 B) : A(A), B(B), mode(0), y0(std::numeric_limits<float>::max()) {

}

Prism::Prism(const pybind11::kwargs& args) {
    auto points = args["points"].cast<std::vector<float>>();
    assert(points.size() % 2 == 0);
    glm::vec2 previous(points[0], points[1]);
    m_points.push_back(previous);
    m_bounds.min.y = -10;
    m_bounds.max.y = 10;
    for (size_t i = 2; i < points.size(); i+=2) {
        glm::vec2 current(points[i], points[i+1]);
        m_points.push_back(current);
        m_edges.push_back(Edge(previous, current));
        previous = current;
        m_bounds.min.x = std::min(m_bounds.min.x, points[i]);
        m_bounds.max.x = std::max(m_bounds.max.x, points[i]);
        m_bounds.min.z = std::min(m_bounds.min.z, points[i+1]);
        m_bounds.max.z = std::max(m_bounds.max.z, points[i+1]);

    }
    m_edges.push_back(Edge(previous, m_points[0]));
    // specify transparent wall
    if (args.contains("open") && !args["open"].is_none()) {
        auto walls = args["open"].cast<pybind11::dict>();
            for (auto &wall : walls) {
                int edgeId = wall.first.cast<int>();
                float y = wall.second.cast<float>();
                m_edges[edgeId].mode = 1;
                m_edges[edgeId].y0 = y;

            }
    }
}