#include "idle.h"
#include "../node.h"
#include "../pyhelper.h"


Idle::Idle(const std::string& id, const std::string& anim, const pybind11::kwargs& args) : State(id, args), m_animation(anim) {

    m_exitState = dictget<std::string>(args, "exit_state", "");
    m_exitOnComplete = dictget<bool>(args, "exit_on_complete", false);
    m_timeOut = dictget<float>(args, "timeout", 0.f);
}

void Idle::init(const pybind11::kwargs& args) {
    State::init(args);
    auto anim = dictget<std::string>(args, "anim", m_animation);
    m_spriteRenderer->setAnimation(anim);
    m_time = 0.f;
}

void Idle::setParent(StateMachine * sm) {
    State::setParent(sm);
    auto node = sm->getNode();

    m_spriteRenderer = dynamic_cast<SpriteRenderer*>(node->getComponent<Renderer>());
}

void Idle::run(double dt) {
    if (!m_exitState.empty()) {
        if (m_exitOnComplete) {
            if (m_spriteRenderer->isComplete()) {
                m_sm->setState(m_exitState);
            }
        } else {
            m_time += static_cast<float>(dt);
            if (m_time >= m_timeOut) {
                m_sm->setState(m_exitState);
            }
        }
    }
}