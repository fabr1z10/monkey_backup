#include "idle.h"
#include "../node.h"

Idle::Idle(const std::string& id, const std::string& anim, const pybind11::kwargs& args) : State(id, args), m_animation(anim) {}

void Idle::init(const pybind11::kwargs& args) {
    State::init(args);
    m_spriteRenderer->setAnimation(m_animation);

}

void Idle::setParent(StateMachine * sm) {
    auto node = sm->getNode();

    m_spriteRenderer = dynamic_cast<SpriteRenderer*>(node->getComponent<Renderer>());
}