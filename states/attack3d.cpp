#include "attack3d.h"
#include "../node.h"
#include <GLFW/glfw3.h>
#include "../pyhelper.h"
#include "../util.h"


extern GLFWwindow* window;


Attack3D::Attack3D(const std::string& id, const pybind11::kwargs& args) : State(id, args), m_currentAnim(0) {
	for (const auto& anim : args["anims"])
		m_anims.push_back(anim.cast<std::string>());
	//m_anims = args["anims"].cast<std::vector<std::string>>();
	m_exitState = args["exit_state"].cast<std::string>();
	m_maxSpeed = args["speed"].cast<float>();
	m_gravity = dictget<float>(args, "gravity", 0.0f);
	m_accelerationTime = dictget<float>(args, "acc_time", 0.1f);
	m_acceleration = m_maxSpeed / m_accelerationTime;
	m_velocityThreshold =0.01f;
	m_keyCombo = args["key"].cast<unsigned>();
}

void Attack3D::init(const pybind11::kwargs& args) {
	if (m_controller->grounded()) {
		m_renderer->setAnimation(m_anims[m_currentAnim]);
	} else {
		if (!m_jumpAnim.empty())
			m_renderer->setAnimation(m_jumpAnim);
	}
}

//void Attack3D::keyCallback(int key) {
//	if (m_controller->grounded() && (key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT)) {
//		m_sm->setState(m_exitState);
//	}
//	m_node->setFlipX(key == GLFW_KEY_LEFT);
//
//	State::keyCallback(key);
//}


void Attack3D::setParent(StateMachine * sm) {

	State::setParent(sm);
	m_renderer = dynamic_cast<AnimatedRenderer*>(sm->getNode()->getComponent<Renderer>());
	m_node = sm->getNode();

	m_controller = dynamic_cast<Controller3D*>(m_sm->getNode()->getComponent<Controller>());
	assert(m_controller != nullptr);

	m_dynamics = m_sm->getNode()->getComponent<Dynamics>();
	assert(m_dynamics != nullptr);

	m_keypad = m_node->getComponent<KeyPad>();
	assert(m_keypad != nullptr);
}

void Attack3D::run(double dt) {
	if (!m_controller->grounded()) {			// jumping
		auto dtf = static_cast<float>(dt);

		glm::vec3 a(0.0f);
		a.y = -m_gravity;

		// acceleration along x axis
		if (m_keypad->lor()) {
			// left or right
			a.x = m_acceleration; // (m_left ? -1.f : 1.f) * m_acceleration;
		}
		// acceleration along z axis
		if (m_keypad->up()) {
			a.z = -m_acceleration; // going inside screen (up)
		} else if (m_keypad->down()) {
			a.z = m_acceleration;
		}
		// no direction keys pressed --> apply deceleration
		if (!m_keypad->lor()) {            // test first left or right
			if (fabs(m_dynamics->m_velocity.x) > m_velocityThreshold) {
				a.x = -m_acceleration;
			} else {
				a.x = 0.0f;
				m_dynamics->m_velocity.x = 0.0f;
			}
		}
		if (!(m_keypad->uod())) {			// then test up or down
			if (fabs(m_dynamics->m_velocity.z) > m_velocityThreshold) {
				a.z = - signf(m_dynamics->m_velocity.z) * m_acceleration;
			} else {
				a.z = 0.0f;
				m_dynamics->m_velocity.z = 0.0f;
			}
		}

		m_dynamics->m_velocity += a * dtf;

		// limit horizontal vel to max speed
		if (fabs(m_dynamics->m_velocity.x) > m_maxSpeed) {
			m_dynamics->m_velocity.x = signf(m_dynamics->m_velocity.x) * m_maxSpeed;
		}
		if (fabs(m_dynamics->m_velocity.z) > m_maxSpeed) {
			m_dynamics->m_velocity.z = signf(m_dynamics->m_velocity.z) * m_maxSpeed;
		}


		auto delta = m_dynamics->m_velocity * dtf;

		m_controller->move(delta, false);

	}








	if (/*m_changeStateAfterAnim &&*/m_renderer->isComplete()) {
		if (m_controller->grounded() && m_keypad->check(m_keyCombo)) {
			m_currentAnim++;
			if (m_currentAnim >= m_anims.size()) {
				m_currentAnim = 0;
			}
			m_renderer->setAnimation(m_anims[m_currentAnim]);

		} else {
			m_currentAnim = 0;
			m_sm->setState(m_exitState);
		}

	}


}