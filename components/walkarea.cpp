#include "walkarea.h"
#include <pybind11/stl.h>
#include "../util.h"
#include "../node.h"
#include "../models/rawmodel.h"
#include <algorithm>

WalkArea::WalkArea(const pybind11::kwargs& args) {

    auto outline = args["outline"].cast<std::vector<float>>();

    for (size_t i = 0; i < outline.size(); i += 2) {
        glm::vec2 current(outline[i], outline[i+1]);
        m_outline.emplace_back(current);
    }

    if (args.contains("holes")) {
        for (const auto& o : args["holes"]) {
            auto hole = o.cast<std::vector<float>>();
            std::vector<glm::vec2> h;
            for (size_t i = 0; i < hole.size(); i += 2) {
                h.emplace_back(outline[i], outline[i+1]);
            }
            m_holes.push_back(h);

        }
    }


    // prepare edge data
    glm::vec2 uvec = glm::normalize(m_outline.front() - m_outline.back());
    for (size_t i = 0; i < m_outline.size() - 1; ++i) {
        EdgeData edge;
        edge.start = m_outline[i];
        edge.end = m_outline[i+1];
        edge.unitVec = glm::normalize(m_outline[i+1] - m_outline[i]);
        edge.normal = rot90(edge.unitVec, false);
        edge.length = glm::length(m_outline[i+1] - m_outline[i]);
        edge.normalInAtStart = glm::normalize(edge.unitVec - uvec);
        if (i > 0)
            m_edgeData.back().normalInAtEnd = edge.normalInAtStart;
        m_edgeData.push_back(edge);
    }

}

glm::vec2 WalkArea::closestPointIn(glm::vec2 P) {
    if (isPointInside(P)) {
        return P;
    }

    glm::vec2 closestPointSoFar;
    float distanceSoFar = std::numeric_limits<float>::infinity();
    for (const auto& edge : m_edgeData) {
        auto dp = glm::dot(P - edge.start, edge.unitVec);
        glm::vec2 currentPoint;
        float currentLength;
        glm::vec2 currentNormal;
        if (dp <= 0.f) {
            currentPoint = edge.start;
            currentNormal = edge.normalInAtStart;
        } else if (dp >= edge.length) {
            currentPoint = edge.end;
            currentNormal = edge.normalInAtEnd;
        } else {
            currentPoint = edge.start + dp * edge.unitVec;
            currentNormal = edge.normal;
        }
        currentLength = glm::length(P - currentPoint);
        if (currentLength < distanceSoFar) {
            closestPointSoFar = currentPoint + currentNormal * 0.1f;
            distanceSoFar = currentLength;
        }
    }
    return closestPointSoFar;

}

bool WalkArea::isPointInside(glm::vec2 p) {
    bool isInOutline = pnpoly(m_outline, p);
    if (!isInOutline)
        return false;
    bool isInHole = false;
    for (const auto& h : m_holes) {
        isInHole |= pnpoly(h, p);
        if (isInHole)
            return false;
    }
    return true;
}

std::vector<glm::vec2> WalkArea::getPath(glm::vec2 start, glm::vec2 target) {
    std::cout << "calculating path between (" << start.x << ", " << start.y << ") --> (" << target.x << ", " << target.y << ")\n";
    int startId = addNode(start);
    int endId = addNode(target);
    std::vector<int> ciao;
    m_graph->shortestPath(startId, endId, ciao);
    std::vector<glm::vec2> path;
    for (auto it = ciao.rbegin(); it != ciao.rend(); it++) {
        auto p = m_graph->getNode(*it);
        path.push_back(p);
        std::cout << "(" << p.x << ", " << p.y << ")" << ", ";
    }
    std::cout << "\n";
    m_graph->removeNode(endId);
    m_graph->removeNode(startId);
    return path;
}

void WalkArea::start() {

    // step 1. get all the edges for LOS test
    // edges include:
    //   - outline edges
    //   - active hole edges
    //   - walls
    //
    // step 2. add all nodes. For each node, check LOS with all other nodes
    // nodes include:
    //   - concave outline nodes
    //   - convex hole nodes
    //   - internal wall nodes

    // step 1.
    m_graph = std::make_shared<Graph<glm::vec2>>();

    m_nodeCount = 0;
    int offset = 0;
    addPath(m_outline, false, offset);
    offset += m_outline.size();
    for (const auto& h : m_holes) {
        addPath(h, true, offset);
        offset += h.size();
    }

    std::vector<float> vertices;
    std::vector<unsigned> elements;
    unsigned i = 0;
    for (const auto& p : m_outline) {
        vertices.insert(vertices.end(), {p.x, p.y, 1.f, 1.f, 1.f, 1.f, 1.f});
        unsigned j = (i+1) % m_outline.size();
        elements.insert(elements.end(), {i++, j});
    }
    auto model = std::make_shared<RawModel>(ShaderType::SHADER_COLOR, GL_LINES, vertices, elements);
    auto node = std::make_shared<Node>();
    node->setModel(model);
    m_node->add(node);

}

bool WalkArea::checkEdges(const std::vector<glm::vec2> & path, glm::vec2 A, glm::vec2 B) {
    for (size_t i = 1; i < path.size(); ++i) {
        float t{0.f};
        if (seg2seg(A, B, path[i-1], path[i], t)) {
            return true;
        }
    }
    return false;
}

void WalkArea::addPath(const std::vector<glm::vec2> & path, bool isHole, int offset) {
    // consider special case of path made of only 2 points (segment)
    if (path.size() == 2) {
        if (isHole) {
            addNode(path[0]);
            addNode(path[1]);
        }
        return;
    }
    size_t ip = path.size() - 1;
    size_t ic = 0;
    size_t in = 1;
    do {
        glm::vec2 edgeNext = path[in] - path[ic];
        glm::vec2 edgePrev = path[ic] - path[ip];
        float flip = isHole ? -1.0f : 1.0f;
        auto f = cross2D(edgePrev, edgeNext) * flip;
        if (f < 0) {
            glm::vec2 u = glm::normalize(edgeNext);
            glm::vec2 v = glm::normalize(-edgePrev);
            glm::vec2 w = glm::normalize(u+v);
            addNode(path[ic] - w * 0.1f);
        }
        ip = ic;
        ic = in;
        in = (in + 1) % m_outline.size();
    } while (ic != 0);
}

int WalkArea::addNode(glm::vec2 point) {
    // adds a node to the graph
    // and an edge from the new node to every other node that is in line of sight with it
    //int id = m_graph->getVertexCount();

    int id = m_graph->addNode(point);

    for (const auto& node : m_graph->getNodes()) {
        if (node.first != id) {
            glm::vec2 P = node.second;
            auto intersection = checkEdges(m_outline, point, P);
            if (!intersection) {
                for (const auto& h : m_holes) {
                    intersection = checkEdges(h, point, P);
                    if (intersection) break;
                }
            }

            if (!intersection) {
                // check mid point. The segment might connect two vertices of the polygon but lie completely outside of it
                // quick way is to check if mid point of the segment lies inside the poly
                m_graph->addEdge(id, node.first, glm::length(point-P));
            }

        }
    }
    return id;
}