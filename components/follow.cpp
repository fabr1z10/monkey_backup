#include "follow.h"
#include "../node.h"

Follow::Follow (std::shared_ptr<Camera> cam, pybind11::tuple& c, pybind11::tuple& a) : Component(), m_cam(cam) {
	m_relativePos = glm::vec3(c[0].cast<float>(), c[1].cast<float>(), c[2].cast<float>());
	m_up = glm::vec3(a[0].cast<float>(), a[1].cast<float>(), a[2].cast<float>());+
	m_previous = glm::mat4(0.0f);
}

void Follow::update (double dt) {
	if (!m_active) return;
	auto matrix = m_node->getWorldMatrix();
	if (m_previous - matrix != glm::mat4(0.0f)) {
		glm::vec3 eye = matrix * glm::vec4(m_relativePos, 1.0f);
		auto dir = glm::normalize(m_node->getWorldPosition() - eye);
		m_cam->setPosition(eye, dir, m_up);
		m_previous = matrix;
	}

}