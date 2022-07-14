#include "bounds.h"


Bounds::Bounds() : min(glm::vec3(0.0f)), max(glm::vec3(0.0f)) {

}

void Bounds::addPoint(glm::vec2 P) {
	min.x = std::min(min.x, P.x);
	max.x = std::max(max.x, P.x);
	min.y = std::min(min.y, P.y);
	max.y = std::max(max.y, P.y);
}

void Bounds::addPoint(glm::vec3 P) {
	min.x = std::min(min.x, P.x);
	max.x = std::max(max.x, P.x);
	min.y = std::min(min.y, P.y);
	max.y = std::max(max.y, P.y);
	min.z = std::min(min.z, P.z);
	max.z = std::max(max.z, P.z);
}

bool Bounds::isVoid() const {
	return (max.x == min.x && max.y == min.y && max.z == min.z);
}

bool Bounds::intersect2D(Bounds& other) const {
	return !(other.min.x > max.x || other.max.x < min.x ||
			 other.min.y > max.y || other.max.y < min.y);
}

void Bounds::transform(const glm::mat4& m) {
	glm::vec3 P[] = {
			glm::vec3(min.x, min.y, min.z),
			glm::vec3(max.x, min.y, min.z),
			glm::vec3(max.x, max.y, min.z),
			glm::vec3(min.x, max.y, min.z),
			glm::vec3(min.x, min.y, max.z),
			glm::vec3(max.x, min.y, max.z),
			glm::vec3(max.x, max.y, max.z),
			glm::vec3(min.x, max.y, max.z),
	};

	float inf = std::numeric_limits<float>::infinity();
	min = glm::vec3(inf, inf, inf);
	max = glm::vec3(-inf, -inf, -inf);
	for (int i = 0; i < 8; i++) {
		glm::vec3 Pt = glm::vec3(m * glm::vec4(P[i], 1.0f));
		if (Pt.x < min.x)
			min.x = Pt.x;
		else if (Pt.x > max.x)
			max.x = Pt.x;
		if (Pt.y < min.y)
			min.y = Pt.y;
		else if (Pt.y > max.y)
			max.y = Pt.y;
		if (Pt.z < min.z)
			min.z = Pt.z;
		else if (Pt.z > max.z)
			max.z = Pt.z;
	}
}