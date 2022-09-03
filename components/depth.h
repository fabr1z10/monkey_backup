#pragma once

#include "../component.h"
#include <glm/glm.hpp>
#include <pybind11/pybind11.h>

class Depth : public Component {
public:
	Depth(const pybind11::kwargs&);
	void update(double) override;
private:
	glm::vec3 m_depth;
	glm::vec3 m_scale;
};
