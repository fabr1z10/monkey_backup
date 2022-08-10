#include "walk.h"
#include "../node.h"
#include "../components/walkarea.h"
#include <glm/gtx/transform.hpp>

Walk::Walk(const pybind11::kwargs& args) : NodeAction(args) {
    m_targetPos = args["pos"].cast<glm::vec2>();
    m_speed = args["speed"].cast<float>();
}

void Walk::start() {
    NodeAction::start();
    auto current = m_node;
    WalkArea* wa = nullptr;
    while (current != nullptr) {
        wa = current->getComponent<WalkArea>();
        if (wa != nullptr) break;
        current = current->getParent();
    }
    if (wa == nullptr) {
        std::cout << "character not associated to a walkarea!\n";
    } else {
        auto initialPosition = m_node->getWorldPosition();
        std::cout << "Request walking from " << initialPosition.x << ", " << initialPosition.y << " to " << m_targetPos.x << ", " << m_targetPos.y << "\n";
        auto start = wa->closestPointIn(initialPosition);
        auto end = wa->closestPointIn(m_targetPos);
        if (glm::length(end-start) <= 0.1f) {
            m_currentSegment = -1;
            return;
        }
        m_node->setPosition(start.x, start.y, initialPosition.z);
        // generate path
        auto path = wa->getPath(start, end);
        for (size_t i = 1; i < path.size(); ++i) {
            PathSegment p;
            p.start = path[i-1];
            p.end = path[i];
            p.length = glm::length(path[i] - path[i-1]);
            p.direction = glm::normalize(path[i] - path[i-1]);
            if (fabs(p.direction.x) > fabs(p.direction.y)) {
                p.animation = "idle_e";
                p.flip = p.direction.x < 0;
            } else {
                p.animation = p.direction.y > 0.f ? "idle_n" : "idle_s";
                p.flip = false;
            }
            m_segments.push_back(p);
        }
        m_currentSegment = 0;
    }
    m_currentLength = 0.f;
}


int Walk::run(double dt) {
    if (m_currentSegment == -1) return 0;
    auto dtf = static_cast<float>(dt);
    const auto& seg = m_segments[m_currentSegment];
    glm::vec2 delta = seg.direction * m_speed * dtf;
    m_currentLength += glm::length(delta);
    if (m_currentLength >= seg.length) {
        m_node->setPosition(seg.end.x, seg.end.y, 0.f);
        m_currentSegment++;
        if (m_currentSegment >= m_segments.size()) {
            return 0;
        }
        m_currentLength = 0.f;
    } else {
        m_node->move(glm::translate(glm::vec3(delta, 0.f)));
    }
    return 1;

}