#include "autowalk2d.h"
#include "../pyhelper.h"
#include "../node.h"

AutoWalk2D::AutoWalk2D(const std::string& id, const pybind11::kwargs& kwargs): Walk2D(id, kwargs) {
}

void AutoWalk2D::init(const pybind11::kwargs &kwargs) {
    m_timer = 0.f;
    for (auto& e : kwargs["events"]) {
        AutoInputEvent event;
        auto obj = e.cast<pybind11::object>();
        event.t = e["t"].cast<float>();
        event.left = dictget<bool>(obj, "left", false);
        event.right = dictget<bool>(obj, "right", false);
        event.up = dictget<bool>(obj, "up",false);
        event.callback = dictget<pybind11::function>(obj, "callback", pybind11::function());
        m_inputEvents.push_back(event);

    }

    Walk2D::init(kwargs);
}

void AutoWalk2D::run(double dt) {
    m_timer += static_cast<float>(dt);
    Walk2D::run(dt);



}

void AutoWalk2D::control() {
    if (!m_inputEvents.empty()) {
        auto& e = m_inputEvents.front();
        if (m_timer >= e.t) {
            m_left = e.left;
            m_right = e.right;
            m_up = e.up;
            if (e.callback) e.callback();
            m_inputEvents.pop_front();
        }
    }
    m_node->setFlipX(m_left);
    //m_spriteRenderer->flipHorizontal(m_left);
}