#pragma once

#include <glm/glm.hpp>

struct Bounds {
	glm::vec3 min;
	glm::vec3 max;
	Bounds();

	bool isVoid() const;
	void transform(const glm::mat4& m);
	bool intersect2D(Bounds& other) const;


};