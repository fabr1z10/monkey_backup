#pragma once

#include "../intersector.h"

class Intersector3D : public Intersector {
public:
	Intersector3D();
private:
	CollisionReport AABB2(const Shape*, const Shape*, const glm::mat4&, const glm::mat4&);

};
