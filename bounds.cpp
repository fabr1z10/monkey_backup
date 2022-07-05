#include "bounds.h"


Bounds::Bounds() : min(glm::vec3(0.0f)), max(glm::vec3(0.0f)) {

}

bool Bounds::isVoid() const {
	return (max.x == min.x && max.y == min.y && max.z == min.z);
}

bool Bounds::intersect2D(Bounds& other) const {
	return !(other.min.x > max.x || other.max.x < min.x ||
			 other.min.y > max.y || other.max.y < min.y);
}