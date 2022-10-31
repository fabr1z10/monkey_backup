#include "hit.h"
#include "../node.h"
#include "../pyhelper.h"

Hit::Hit(const std::string& id, const pybind11::kwargs& kwargs) : State(id, kwargs) {
    m_initialVelocity = kwargs["velocity"].cast<glm::vec2>();
    m_animFly = kwargs["anim"].cast<std::string>();
    m_timeout = kwargs["timeout"].cast<float>();
    m_exitState = kwargs["exit_state"].cast<std::string>();
}

void Hit::init(const pybind11::kwargs &args) {
    State::init(args);
    bool right = args["dir"].cast<bool>();
    float k = (!m_node->getFilpX() == right ? 1.f : -1.f);
    m_dynamics->m_velocity = glm::vec3 (k * m_initialVelocity.x, m_initialVelocity.y, 0.f);
    m_animatedRenderer->setAnimation(m_animFly);
    m_timer = 0.f;
}

void Hit::run(double dt) {


    auto dtf = static_cast<float>(dt);
    m_timer += dtf;

    auto delta = m_dynamics->m_velocity * dtf;

    m_controller->move(delta, false);
    if (m_timer >= m_timeout) {
        m_sm->setState(m_exitState);
    }
}

void Hit::setParent(StateMachine * sm) {
    State::setParent(sm);
    m_node = sm->getNode();

    m_controller = dynamic_cast<Controller2D*>(m_sm->getNode()->getComponent<Controller>());
    assert(m_controller != nullptr);

    m_dynamics = m_sm->getNode()->getComponent<Dynamics>();
    assert(m_dynamics != nullptr);

    m_animatedRenderer = dynamic_cast<AnimatedRenderer*>(m_node->getComponent<Renderer>());
}