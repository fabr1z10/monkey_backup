#include "hit3d.h"
#include "../node.h"

Hit3D::Hit3D(const std::string& id, const pybind11::kwargs& kwargs) : State(id, kwargs) {
	m_speed = kwargs["speed"].cast<float>();
	m_verticalSpeed = 0.f;
	m_anim = kwargs["anim"].cast<std::string>();
	m_gravity = dictget<float>(kwargs, "gravity", 0.0f);
	m_timeToStop = kwargs["time"].cast<float>();

	m_mode = kwargs["mode"].cast<int>();

	if (m_mode == 1) {
	    m_verticalSpeed = kwargs["vertical_speed"].cast<float>();
	}
	m_exitState = kwargs["exit_state"].cast<std::string>();

}

void Hit3D::setParent(StateMachine * sm) {
	State::setParent(sm);
	m_controller = dynamic_cast<Controller3D*>(m_sm->getNode()->getComponent<Controller>());
	assert(m_controller != nullptr);
	m_dynamics = m_sm->getNode()->getComponent<Dynamics>();
	assert(m_dynamics != nullptr);
	m_animatedRenderer = dynamic_cast<AnimatedRenderer*>(m_sm->getNode()->getComponent<Renderer>());

}

void Hit3D::init(const pybind11::kwargs &args) {
	// where is the hitter relative to the hit
	auto xOther = args["x"].cast<float>();
	float xSelf = m_sm->getNode()->getWorldPosition().x;
	m_actualSpeed = ((xSelf >= xOther) == m_sm->getNode()->getFilpX()) ? -m_speed : m_speed;
	State::init(args);
	m_animatedRenderer->setAnimation(m_anim);
	m_ax = (m_mode == 0 ? -m_actualSpeed / m_timeToStop : 0.f);
	m_dynamics->m_velocity = glm::vec3(m_actualSpeed, m_verticalSpeed, 0.f);
}

void Hit3D::run(double dt) {
	auto dtf = static_cast<float>(dt);
	//m_dynamics->m_velocity = glm::vec3(m_actualSpeed, 0.f, 0.f);
	auto delta = m_dynamics->m_velocity * dtf;
	m_dynamics->m_acceleration = glm::vec3(m_ax,  -m_gravity, 0.f);
	//m_dynamics->update(dt);
	m_dynamics->m_velocity += m_dynamics->m_acceleration * dtf;

	m_controller->move(delta, false);
	// when horizontal velocity is 0 -> exit

	// exit condition
	if (m_mode == 0 && fabs(m_dynamics->m_velocity.x) < 0.001f) {
		m_sm->setState(m_exitState);
	} else if (m_mode == 1 && m_controller->grounded()) {
	    m_animatedRenderer->setAnimation(m_exitState);
	    m_dynamics->m_velocity = glm::vec3(0.f);
	}

}