#include "actions.h"
#include "../node.h"
#include "../engine.h"
#include <glm/gtx/transform.hpp>
#include "../pyhelper.h"

MoveBy::MoveBy(const pybind11::kwargs& args) {
    m_node = nullptr;
    if (args.contains("id")) {
        m_id = args["id"].cast<int>();
    } else {
        m_node = args["node"].cast<std::shared_ptr<Node>>().get();
    }
    auto dx = dictget<float>(args, "x", 0.f);
    auto dy = dictget<float>(args, "y", 0.f);
    auto dz = dictget<float>(args, "z", 0.f);
    auto t = args["t"].cast<float>();
    m_distance = sqrt(dx * dx + dy * dy);
    m_delta = glm::vec3(dx, dy, 0.f);
    m_unitVec = glm::normalize(m_delta);
    m_speed = m_distance / t;
}

void MoveBy::start() {
    if (m_node == nullptr)
        m_node = Engine::instance().getNode(m_id).get();
    m_endPoint = m_node->getWorldPosition() + m_delta;
    m_distanceTraveled = 0.f;
}


int MoveBy::run(double dt) {
    auto dtf = static_cast<float>(dt);
    auto dist = m_speed * dtf;
    m_distanceTraveled += dist;
    auto delta = m_unitVec * dist;
    if (m_distanceTraveled >= m_distance) {
        m_node->setPosition(m_endPoint.x, m_endPoint.y, m_endPoint.z);
        return 0;
    }
    m_node->move(glm::translate(delta));
    return 1;
}