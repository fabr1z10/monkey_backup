#include "hotspot.h"
#include "../pyhelper.h"
#include "../node.h"
#include "hotspotmanager.h"

HotSpot::HotSpot(std::shared_ptr<Shape> shape, const pybind11::kwargs& args) : m_shape(shape), m_manager(nullptr) {
    m_onEnter = dictget<pybind11::function>(args, "on_enter", pybind11::function());
    m_onLeave = dictget<pybind11::function>(args, "on_leave", pybind11::function());
    m_priority = dictget<int>(args, "priority", 0);
}

void HotSpot::enter() {
    m_onEnter(m_node);
}

void HotSpot::exit() {
    m_onLeave(m_node);
}


void HotSpot::start() {
    auto node = m_node;
    while (node) {
        auto hs = node->getComponent<HotSpotManager>();
        if (hs != nullptr) {
            m_manager = hs;
            hs->add(this);
            break;
        }
        node = node->getParent();
    }
    assert(m_manager != nullptr);


}

bool HotSpot::isMouseInside(glm::vec2 pos) {
    const auto& wt = glm::inverse(m_node->getWorldMatrix());
    glm::vec3 lpos(wt * glm::vec4(pos.x, pos.y, 0.0f, 1.0f));
    return m_shape->isInside(lpos);
}

HotSpot::~HotSpot() {
    if (m_manager != nullptr) {
        m_manager->remove(this);
    }
}
