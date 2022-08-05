#include "selfdestroy.h"
#include "../pyhelper.h"
#include "../node.h"

SelfDestroy::SelfDestroy(const pybind11::kwargs& args) : Component() {
    m_timeout = dictget<float>(args, "timeout", std::numeric_limits<float>::infinity());
    m_yMin = dictget<float>(args, "y_min", -std::numeric_limits<float>::infinity());
}

void SelfDestroy::start() {
    m_timer = 0.f;
}

void SelfDestroy::update(double dt) {
    m_timer += static_cast<float>(dt);
    if (m_timer > m_timeout || (m_node->getWorldPosition().y < m_yMin)) {
        m_node->remove();
    }



}