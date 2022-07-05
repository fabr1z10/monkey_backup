#include "convexpoly.h"


ConvexPoly::ConvexPoly(const py::array_t<float>& input) {
	glm::vec2 previous;
	for (size_t i = 0; i < input.size(); i+=2) {
		glm::vec2 current(input.at(i), input.at(i+1));
		m_points.push_back(current);
		if (i > 0) {
			m_edges.push_back(current - previous);
		}
		previous = current;
	}
	m_edges.push_back(m_points.front() - m_points.back());
}

glm::vec2 ConvexPoly::project(glm::vec2 axis, const glm::mat4 & t) const {

	glm::vec2 out (std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity());
	for (const auto& p : m_points) {
		glm::vec2 pw = t * glm::vec4(p, 0.0f, 1.0f);
		float pr = glm::dot (pw, axis);
		out.x = std::min (out.x, pr);
		out.y = std::max (out.y, pr);
	}
	return out;
}

Rect::Rect(float w, float h, const py::kwargs& kwargs) {
	float ox{0.0f};
	float oy{0.0f};
	if (kwargs) {
		if (kwargs.contains("ox")) {
			ox = kwargs["ox"].cast<float>();
		}
		if (kwargs.contains("oy")) {
			oy = kwargs["oy"].cast<float>();
		}
	}
	m_points.emplace_back(ox, oy);
	m_points.emplace_back(ox + w, oy);
	m_points.emplace_back(ox + w, oy + h);
	m_points.emplace_back(ox, oy + h);
	m_edges.emplace_back(w, 0);
	m_edges.emplace_back(0, h);
	m_edges.emplace_back(-w, 0);
	m_edges.emplace_back(0, -h);


}