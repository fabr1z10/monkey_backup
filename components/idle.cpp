#include "idle.h"
#include "../node.h"
#include "../pyhelper.h"


Idle::Idle(const std::string& id, const std::string& anim, const pybind11::kwargs& args) : State(id, args), m_animation(anim) {}

void Idle::init(const pybind11::kwargs& args) {
    State::init(args);
    auto anim = dictget<std::string>(args, "anim", m_animation);
    m_spriteRenderer->setAnimation(anim);

}

void Idle::setParent(StateMachine * sm) {
    auto node = sm->getNode();

    m_spriteRenderer = dynamic_cast<SpriteRenderer*>(node->getComponent<Renderer>());
}