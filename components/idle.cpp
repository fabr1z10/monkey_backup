#include "idle.h"
#include "../node.h"

Idle::Idle(const std::string& id, const std::string& anim, const pybind11::kwargs& args) : State(id, args), m_animation(anim) {}

void Idle::init() {
    State::init();
    m_spriteRenderer->setAnimation(m_animation);

}

void Idle::setParent(StateMachine * sm) {
    auto node = sm->getNode();

    m_spriteRenderer = dynamic_cast<SpriteRenderer*>(node->getComponent<Renderer>());
}