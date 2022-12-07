#pragma once


#include "../raycaster.h"

class RayCaster3D : public RayCaster {
public:
	RayCaster3D();
private:
	RayCastHit raycastAABB(const glm::vec3& A, const glm::vec3& B, const Shape *s, const glm::mat4 &t);
};