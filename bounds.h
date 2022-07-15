#pragma once

#include <glm/glm.hpp>

struct Bounds {
	glm::vec3 min;
	glm::vec3 max;
	Bounds();
	Bounds(glm::vec3);
    void addPoint(glm::vec2);
    void addPoint(glm::vec3);
	bool isVoid() const;
	void transform(const glm::mat4& m);
	bool intersect2D(Bounds& other) const;
    void expandWith(const Bounds&);

};