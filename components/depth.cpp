#include "depth.h"
#include "../node.h"

Depth::Depth(const pybind11::kwargs& args) {
	if (args.contains("depth")) {
		m_depth = args["depth"].cast<glm::vec3>();
	}
	if (args.contains("scale")) {
		m_scale = args["scale"].cast<glm::vec3>();
	}
}

void Depth::update(double) {
	auto pos = m_node->getLocalPosition();
	auto z = m_depth.x * pos.x + m_depth.y * pos.y + m_depth.z;
	m_node->setPosition(pos.x, pos.y, z);

}