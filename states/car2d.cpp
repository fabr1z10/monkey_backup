#include "car2d.h"
#include "../pyhelper.h"
#include "../node.h"
#include <GLFW/glfw3.h>
#include "../util.h"
#include <glm/gtx/transform.hpp>

extern GLFWwindow* window;

Car2D::Car2D(const std::string& id, const pybind11::kwargs& kwargs) : State(id, kwargs) {
	m_maxSpeed = kwargs["speed"].cast<float>();
	m_accelerationTime = dictget<float>(kwargs, "acc_time", 0.1f);
	m_acceleration = m_maxSpeed / m_accelerationTime;
	m_rotationSpeed = dictget<float>(kwargs, "rotation_speed", 0.1f);

}




void Car2D::setParent(StateMachine * sm) {
	State::setParent(sm);
	m_node = sm->getNode();

	//m_controller = m_sm->getNode()->getComponent<Controller>();
	//assert(m_controller != nullptr);

	m_dynamics = m_sm->getNode()->getComponent<Dynamics>();
	assert(m_dynamics != nullptr);
}


void Car2D::run(double dt) {
	auto dtf = static_cast<float>(dt);

	bool left = glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS;
	bool right = glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS;
	bool fwd = glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS;
	bool back = glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS;

	glm::vec3 a(0.0f);

	if (fwd) {
		a.y = m_acceleration;
	} else if (back) {
		a.y = -m_acceleration;
	} else {
		if (fabs(m_dynamics->m_velocity.y) > 0.1f) {
			a.y = -sign(m_dynamics->m_velocity.y)*m_acceleration;
		} else {
			a.y = 0.0f;
			m_dynamics->m_velocity.y = 0.0f;
		}
	}
	m_dynamics->m_velocity += a * dtf;
	if (fabs(m_dynamics->m_velocity.y) > m_maxSpeed) {
		m_dynamics->m_velocity.y = sign(m_dynamics->m_velocity.y) * m_maxSpeed;
	}

	auto delta = m_dynamics->m_velocity * dtf;
	glm::mat4 rotate(1.0f);
	if (left) {
		rotate = glm::rotate(m_rotationSpeed * dtf, glm::vec3(0.f, 0.f, 1.f));
	} else if (right) {
		rotate = glm::rotate(-m_rotationSpeed * dtf, glm::vec3(0.f, 0.f, 1.f));
	}
	//m_controller->move(delta);
	m_node->move(rotate*glm::translate(delta));
}

