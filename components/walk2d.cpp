#include "walk2d.h"
#include "../pyhelper.h"
#include "../node.h"
#include <GLFW/glfw3.h>
#include "../util.h"

extern GLFWwindow* window;

void Walk2D::init() {

}

Walk2D::Walk2D(const std::string& id, const pybind11::kwargs& kwargs) : State(id) {
	m_gravity = dictget<float>(kwargs, "gravity", 0.0f);
	m_jumpHeight = kwargs["jump_height"].cast<float>();
    m_timeToJumpApex = kwargs["time_to_jump_apex"].cast<float>();
    m_jumpVelocity = (m_jumpHeight + 0.5f * m_gravity * m_timeToJumpApex * m_timeToJumpApex) / m_timeToJumpApex;
	m_maxSpeed = kwargs["speed"].cast<float>();
	m_accelerationTime = dictget<float>(kwargs, "acc_time", 0.1f);
	m_acceleration = m_maxSpeed / m_accelerationTime;

	std::cout << "gravity = " << m_gravity << "\n";
}


void Walk2D::setParent(StateMachine * sm) {
	State::setParent(sm);
	m_node = sm->getNode();

	m_controller = dynamic_cast<Controller2D*>(m_sm->getNode()->getComponent<Controller>());
	assert(m_controller != nullptr);

	m_dynamics = m_sm->getNode()->getComponent<Dynamics>();
	assert(m_dynamics != nullptr);
}

void Walk2D::run(double dt) {
	auto dtf = static_cast<float>(dt);

	bool left = glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS;
	bool right = glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS;
    bool up = glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS;


    if (m_controller->grounded()) {
        if (up) {
            m_dynamics->m_velocity.y = m_jumpVelocity;
        } else {
            m_dynamics->m_velocity.y = 0.0f;
        }
    }

	glm::vec3 a(0.0f);
	a.y = -m_gravity;

	if (left || right) {
		a.x = (left ? -1.f : 1.f) * m_acceleration;
	} else {
		// apply deceleration only if velocity above threshold
		if (fabs(m_dynamics->m_velocity.x) > 0.1f) {
			a.x = -signf(m_dynamics->m_velocity.x) * m_acceleration;
		} else {
			a.x = 0.0f;
			m_dynamics->m_velocity.x = 0.0f;
		}
	}

	m_dynamics->m_velocity += a * dtf;

	// limit horizontal vel to max speed
	if (left || right) {
		if (fabs(m_dynamics->m_velocity.x) > m_maxSpeed) {
			m_dynamics->m_velocity.x = signf(m_dynamics->m_velocity.x) * m_maxSpeed;
		}
	}


	auto delta = m_dynamics->m_velocity * dtf;

	m_controller->move(delta, false);
}