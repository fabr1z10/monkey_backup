#pragma once

#include <glm/glm.hpp>

struct Bounds {
	glm::vec3 min;
	glm::vec3 max;
	Bounds();

	bool isVoid() const;

	bool intersect2D(Bounds& other) const;


};