#include <iostream>
#include "move.h"
#include <pybind11/numpy.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "../node.h"

Move::Move(pybind11::function f) : Component(), m_func(f), m_time(0.0f) {
	std::cout << "qui\n";

}

void Move::update(double dt) {
	m_time += dt;

	auto obj = m_func(m_time).cast<pybind11::array_t<float>>();

	glm::mat4 m = glm::rotate(obj.at(3), glm::vec3(0, 0, 1));
	//std::cout << m_time << ", " << obj.at(3)<< "\n";
	m_node->setModelMatrix(m);



}