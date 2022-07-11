#pragma once

#include "../component.h"
#include <glm/glm.hpp>

class Dynamics : public Component {
public:
	Dynamics() : m_velocity(0.0f), m_acceleration(0.0f) {}
	void start() override {}
	void update(double) override {}

	glm::vec3 m_velocity;
	glm::vec3 m_acceleration;

};