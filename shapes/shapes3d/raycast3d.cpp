#include "raycast3d.h"

#include "aabb3d.h"

RayCaster3D::RayCaster3D() {
	m_functionMap[std::type_index(typeid(AABB3D))] = [&] (const glm::vec3& A, const glm::vec3& B, const Shape* s, const glm::mat4& t) {
		return raycastAABB(A, B, s, t);
	};
};

RayCastHit RayCaster3D::raycastAABB(const glm::vec3 &A, const glm::vec3 &B, const Shape *s, const glm::mat4 &t) {

	RayCastHit hit;

	// get the transformed bounds
	auto b = s->getBounds();
	b.transform(t);

	glm::vec3 dir = (B - A);

	float tmin = (b.min.x - A.x) / dir.x;
	float tmax = (b.max.x - A.x) / dir.x;

	if (tmin > tmax) std::swap(tmin, tmax);

	float tymin = (b.min.y - A.y) / dir.y;
	float tymax = (b.max.y - A.y) / dir.y;

	if (tymin > tymax) std::swap(tymin, tymax);

	if ((tmin > tymax) || (tymin > tmax))
		return hit;

	if (tymin > tmin)
		tmin = tymin;

	if (tymax < tmax)
		tmax = tymax;

	float tzmin = (b.min.z - A.z) / dir.z;
	float tzmax = (b.max.z - A.z) / dir.z;

	if (tzmin > tzmax) std::swap(tzmin, tzmax);

	if ((tmin > tzmax) || (tzmin > tmax))
		return hit;

	if (tzmin > tmin)
		tmin = tzmin;

	if (tzmax < tmax)
		tmax = tzmax;

	hit.collide = true;
	hit.length = tmin;
	return hit;
}