#pragma once

#include "../collision.h"


class Intersector {
public:
	virtual CollisionReport intersect(Shape*, Shape*, const glm::mat4&, const glm::mat4&) = 0;
};
