#pragma once

#include <glm/glm.hpp>

struct CollisionReport {
	CollisionReport() : collide(false) {}
	// tells whether a collision has been found
	bool collide;
	float distance;
	glm::vec3 direction;
};

